#include "whisper.h"
#include "httplib.h"
#include "whisper_params.h"
#include "inference_handler.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable: 4244 4267) // possible loss of data
#endif

using namespace httplib;
bool is_file_exist(const char *fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}

int main(int argc, char **argv) {
  whisper_params params;
  server_params sparams;

  std::mutex whisper_mutex;

  if (whisper_params_parse(argc, argv, params, sparams) == false) {
    whisper_print_usage(argc, argv, params, sparams);
    return 1;
  }

  if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
    whisper_print_usage(argc, argv, params, sparams);
    exit(0);
  }

  if (params.diarize && params.tinydiarize) {
    fprintf(stderr, "error: cannot use both --diarize and --tinydiarize\n");
    whisper_print_usage(argc, argv, params, sparams);
    exit(0);
  }

  // whisper init
  struct whisper_context_params cparams;
  cparams.use_gpu = params.use_gpu;
  struct whisper_context *ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);

  if (ctx == nullptr) {
    fprintf(stderr, "error: failed to initialize whisper context\n");
    return 3;
  }

  // initialize openvino encoder. this has no effect on whisper.cpp builds that don't have OpenVINO configured
  whisper_ctx_init_openvino_encoder(ctx, nullptr, params.openvino_encode_device.c_str(), nullptr);

  Server svr;

  std::string const default_content = "<html>hello</html>";

  // this is only called if no index.html is found in the public --path
  svr.Get("/", [&default_content](const Request &, Response &res) {
    res.set_content(default_content, "text/html");
    return false;
  });

  svr.Post("/inference",  [&](const httplib::Request &req, httplib::Response &res) {
    handleInference(req, res, whisper_mutex, params, ctx, argv[0]);
  });
  svr.Post("/load", [&](const Request &req, Response &res) {
    whisper_mutex.lock();
    if (!req.has_file("model")) {
      fprintf(stderr, "error: no 'model' field in the request\n");
      const std::string error_resp = "{\"error\":\"no 'model' field in the request\"}";
      res.set_content(error_resp, "application/json");
      whisper_mutex.unlock();
      return;
    }
    std::string model = req.get_file_value("model").content;
    if (!is_file_exist(model.c_str())) {
      fprintf(stderr, "error: 'model': %s not found!\n", model.c_str());
      const std::string error_resp = "{\"error\":\"model not found!\"}";
      res.set_content(error_resp, "application/json");
      whisper_mutex.unlock();
      return;
    }

    // clean up
    whisper_free(ctx);

    // whisper init
//    ctx = whisper_init_from_file(model.c_str());
    struct whisper_context_params cparams;

    ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);

    // TODO perhaps load prior model here instead of exit
    if (ctx == nullptr) {
      fprintf(stderr, "error: model init  failed, no model loaded must exit\n");
      exit(1);
    }

    // initialize openvino encoder. this has no effect on whisper.cpp builds that don't have OpenVINO configured
    whisper_ctx_init_openvino_encoder(ctx, nullptr, params.openvino_encode_device.c_str(), nullptr);

    const std::string success = "Load was successful!";
    res.set_content(success, "application/text");

    // check if the model is in the file system
    whisper_mutex.unlock();
  });

  svr.set_exception_handler([](const Request &, Response &res, std::exception_ptr ep) {
    const char fmt[] = "500 Internal Server Error\n%s";
    char buf[BUFSIZ];
    try {
      std::rethrow_exception(std::move(ep));
    } catch (std::exception &e) {
      snprintf(buf, sizeof(buf), fmt, e.what());
    } catch (...) {
      snprintf(buf, sizeof(buf), fmt, "Unknown Exception");
    }
    res.set_content(buf, "text/plain");
    res.status = 500;
  });

  svr.set_error_handler([](const Request &, Response &res) {
    if (res.status == 400) {
      res.set_content("Invalid request", "text/plain");
    } else if (res.status != 500) {
      res.set_content("File Not Found", "text/plain");
      res.status = 404;
    }
  });

  // set timeouts and change hostname and port
  svr.set_read_timeout(sparams.read_timeout);
  svr.set_write_timeout(sparams.write_timeout);

  if (!svr.bind_to_port(sparams.hostname, sparams.port)) {
    fprintf(stderr, "\ncouldn't bind to service socket: hostname=%s port=%d\n\n",
            sparams.hostname.c_str(), sparams.port);
    return 1;
  }

  // Set the base directory for serving static files
  svr.set_base_dir(sparams.public_path);

  // to make it ctrl+clickable:
  printf("\nwhisper service listening at http://%s:%d\n\n", sparams.hostname.c_str(), sparams.port);

  if (!svr.listen_after_bind()) {
    return 1;
  }

  whisper_print_timings(ctx);
  whisper_free(ctx);

  return 0;
}