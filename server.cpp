#include "common.h"

#include "whisper.h"
#include "httplib.h"
#include "json.hpp"
#include "whisper_params.h"
#include <cmath>
#include <fstream>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <cstring>

#if defined(_MSC_VER)
#pragma warning(disable: 4244 4267) // possible loss of data
#endif

using namespace httplib;
using json = nlohmann::json;

namespace {

// Terminal color map. 10 colors grouped in ranges [0.0, 0.1, ..., 0.9]
// Lowest is red, middle is yellow, highest is green.
  const std::vector<std::string> k_colors = {
    "\033[38;5;196m", "\033[38;5;202m", "\033[38;5;208m", "\033[38;5;214m", "\033[38;5;220m",
    "\033[38;5;226m", "\033[38;5;190m", "\033[38;5;154m", "\033[38;5;118m", "\033[38;5;82m",
  };


//  500 -> 00:05.000
// 6000 -> 01:00.000
  std::string to_timestamp(int64_t t, bool comma = false) {
    int64_t msec = t * 10;
    int64_t hr = msec / (1000 * 60 * 60);
    msec = msec - hr * (1000 * 60 * 60);
    int64_t min = msec / (1000 * 60);
    msec = msec - min * (1000 * 60);
    int64_t sec = msec / 1000;
    msec = msec - sec * 1000;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d%s%03d", (int) hr, (int) min, (int) sec, comma ? "," : ".", (int) msec);

    return std::string(buf);
  }

  int timestamp_to_sample(int64_t t, int n_samples) {
    return std::max(0, std::min((int) n_samples - 1, (int) ((t * WHISPER_SAMPLE_RATE) / 100)));
  }

  bool is_file_exist(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
  }

  struct whisper_print_user_data {
    const whisper_params *params;

    const std::vector<std::vector<float>> *pcmf32s;
    int progress_prev;
  };

  std::string
  estimate_diarization_speaker(std::vector<std::vector<float>> pcmf32s, int64_t t0, int64_t t1, bool id_only = false) {
    std::string speaker = "";
    const int64_t n_samples = pcmf32s[0].size();

    const int64_t is0 = timestamp_to_sample(t0, n_samples);
    const int64_t is1 = timestamp_to_sample(t1, n_samples);

    double energy0 = 0.0f;
    double energy1 = 0.0f;

    for (int64_t j = is0; j < is1; j++) {
      energy0 += fabs(pcmf32s[0][j]);
      energy1 += fabs(pcmf32s[1][j]);
    }

    if (energy0 > 1.1 * energy1) {
      speaker = "0";
    } else if (energy1 > 1.1 * energy0) {
      speaker = "1";
    } else {
      speaker = "?";
    }

    //printf("is0 = %lld, is1 = %lld, energy0 = %f, energy1 = %f, speaker = %s\n", is0, is1, energy0, energy1, speaker.c_str());

    if (!id_only) {
      speaker.insert(0, "(speaker ");
      speaker.append(")");
    }

    return speaker;
  }

  void whisper_print_progress_callback(struct whisper_context * /*ctx*/, struct whisper_state * /*state*/, int progress,
                                       void *user_data) {
    int progress_step = ((whisper_print_user_data *) user_data)->params->progress_step;
    int *progress_prev = &(((whisper_print_user_data *) user_data)->progress_prev);
    if (progress >= *progress_prev + progress_step) {
      *progress_prev += progress_step;
      fprintf(stderr, "%s: progress = %3d%%\n", __func__, progress);
    }
  }

  void whisper_print_segment_callback(struct whisper_context *ctx, struct whisper_state * /*state*/, int n_new,
                                      void *user_data) {
    const auto &params = *((whisper_print_user_data *) user_data)->params;
    const auto &pcmf32s = *((whisper_print_user_data *) user_data)->pcmf32s;

    const int n_segments = whisper_full_n_segments(ctx);

    std::string speaker = "";

    int64_t t0 = 0;
    int64_t t1 = 0;

    // print the last n_new segments
    const int s0 = n_segments - n_new;

    if (s0 == 0) {
      printf("\n");
    }

    for (int i = s0; i < n_segments; i++) {
      if (!params.no_timestamps || params.diarize) {
        t0 = whisper_full_get_segment_t0(ctx, i);
        t1 = whisper_full_get_segment_t1(ctx, i);
      }

      if (!params.no_timestamps) {
        printf("[%s --> %s]  ", to_timestamp(t0).c_str(), to_timestamp(t1).c_str());
      }

      if (params.diarize && pcmf32s.size() == 2) {
        speaker = estimate_diarization_speaker(pcmf32s, t0, t1);
      }

      if (params.print_colors) {
        for (int j = 0; j < whisper_full_n_tokens(ctx, i); ++j) {
          if (params.print_special == false) {
            const whisper_token id = whisper_full_get_token_id(ctx, i, j);
            if (id >= whisper_token_eot(ctx)) {
              continue;
            }
          }

          const char *text = whisper_full_get_token_text(ctx, i, j);
          const float p = whisper_full_get_token_p(ctx, i, j);

          const int col = std::max(0, std::min((int) k_colors.size() - 1,
                                               (int) (std::pow(p, 3) * float(k_colors.size()))));

          printf("%s%s%s%s", speaker.c_str(), k_colors[col].c_str(), text, "\033[0m");
        }
      } else {
        const char *text = whisper_full_get_segment_text(ctx, i);

        printf("%s%s", speaker.c_str(), text);
      }

      if (params.tinydiarize) {
        if (whisper_full_get_segment_speaker_turn_next(ctx, i)) {
          printf("%s", params.tdrz_speaker_turn.c_str());
        }
      }

      // with timestamps or speakers: each segment on new line
      if (!params.no_timestamps || params.diarize) {
        printf("\n");
      }
      fflush(stdout);
    }
  }

  std::string
  output_str(struct whisper_context *ctx, const whisper_params &params, std::vector<std::vector<float>> pcmf32s) {
    std::stringstream result;
    const int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
      const char *text = whisper_full_get_segment_text(ctx, i);
      std::string speaker = "";

      if (params.diarize && pcmf32s.size() == 2) {
        const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
        const int64_t t1 = whisper_full_get_segment_t1(ctx, i);
        speaker = estimate_diarization_speaker(pcmf32s, t0, t1);
      }

      result << speaker << text << "\n";
    }
    return result.str();
  }

  void getReqParameters(const Request &req, whisper_params &params) {
    // user model configu.has_fileion
    if (req.has_file("offset-t")) {
      params.offset_t_ms = std::stoi(req.get_file_value("offset-t").content);
    }
    if (req.has_file("offset-n")) {
      params.offset_n = std::stoi(req.get_file_value("offset-n").content);
    }
    if (req.has_file("duration")) {
      params.duration_ms = std::stoi(req.get_file_value("duration").content);
    }
    if (req.has_file("max-context")) {
      params.max_context = std::stoi(req.get_file_value("max-context").content);
    }
    if (req.has_file("prompt")) {
      params.prompt = req.get_file_value("prompt").content;
    }
    if (req.has_file("response-format")) {
      params.response_format = req.get_file_value("response-format").content;
    }
    if (req.has_file("temerature")) {
      params.userdef_temp = std::stof(req.get_file_value("temperature").content);
    }
  }

}  // namespace

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

  svr.Post("/inference", [&](const Request &req, Response &res) {
    // aquire whisper model mutex lock
    whisper_mutex.lock();

    // first check user requested fields of the request
    if (!req.has_file("file")) {
      fprintf(stderr, "error: no 'file' field in the request\n");
      const std::string error_resp = "{\"error\":\"no 'file' field in the request\"}";
      res.set_content(error_resp, "application/json");
      whisper_mutex.unlock();
      return;
    }
    auto audio_file = req.get_file_value("file");

    // check non-required fields
    getReqParameters(req, params);

    std::string filename{audio_file.filename};
    printf("Received request: %s\n", filename.c_str());

    // audio arrays
    std::vector<float> pcmf32;               // mono-channel F32 PCM
    std::vector<std::vector<float>> pcmf32s; // stereo-channel F32 PCM

    // write file to temporary file
    std::ofstream temp_file{filename, std::ios::binary};
    temp_file << audio_file.content;

    // read wav content into pcmf32
    if (!::read_wav(filename, pcmf32, pcmf32s, params.diarize)) {
      fprintf(stderr, "error: failed to read WAV file '%s'\n", filename.c_str());
      const std::string error_resp = "{\"error\":\"failed to read WAV file\"}";
      res.set_content(error_resp, "application/json");
      whisper_mutex.unlock();
      return;
    }
    // remove temp file
    std::remove(filename.c_str());

    printf("Successfully loaded %s\n", filename.c_str());

    // print system information
    {
      fprintf(stderr, "\n");
      fprintf(stderr, "system_info: n_threads = %d / %d | %s\n",
              params.n_threads * params.n_processors, std::thread::hardware_concurrency(), whisper_print_system_info());
    }

    // print some info about the processing
    {
      fprintf(stderr, "\n");
      if (!whisper_is_multilingual(ctx)) {
        if (params.language != "en" || params.translate) {
          params.language = "en";
          params.translate = false;
          fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n",
                  __func__);
        }
      }
      if (params.detect_language) {
        params.language = "auto";
      }
      fprintf(stderr,
              "%s: processing '%s' (%d samples, %.1f sec), %d threads, %d processors, lang = %s, task = %s, %stimestamps = %d ...\n",
              __func__, filename.c_str(), int(pcmf32.size()), float(pcmf32.size()) / WHISPER_SAMPLE_RATE,
              params.n_threads, params.n_processors,
              params.language.c_str(),
              params.translate ? "translate" : "transcribe",
              params.tinydiarize ? "tdrz = 1, " : "",
              params.no_timestamps ? 0 : 1);

      fprintf(stderr, "\n");
    }

    // run the inference
    {

      printf("Running whisper.cpp inference on %s\n", filename.c_str());
      whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

      wparams.strategy = params.beam_size > 1 ? WHISPER_SAMPLING_BEAM_SEARCH : WHISPER_SAMPLING_GREEDY;

      wparams.print_realtime = false;
      wparams.print_progress = params.print_progress;
      wparams.print_timestamps = !params.no_timestamps;
      wparams.print_special = params.print_special;
      wparams.translate = params.translate;
      wparams.language = params.language.c_str();
      wparams.detect_language = params.detect_language;
      wparams.n_threads = params.n_threads;
      wparams.n_max_text_ctx = params.max_context >= 0 ? params.max_context : wparams.n_max_text_ctx;
      wparams.offset_ms = params.offset_t_ms;
      wparams.duration_ms = params.duration_ms;

      wparams.thold_pt = params.word_thold;
      wparams.split_on_word = params.split_on_word;

      wparams.speed_up = params.speed_up;
      wparams.debug_mode = params.debug_mode;

      wparams.tdrz_enable = params.tinydiarize; // [TDRZ]

      wparams.initial_prompt = params.prompt.c_str();

      wparams.greedy.best_of = params.best_of;
      wparams.beam_search.beam_size = params.beam_size;

      wparams.temperature_inc = params.userdef_temp;
      wparams.entropy_thold = params.entropy_thold;
      wparams.logprob_thold = params.logprob_thold;

      whisper_print_user_data user_data = {&params, &pcmf32s, 0};

      // this callback is called on each new segment
      if (!wparams.print_realtime) {
        wparams.new_segment_callback = whisper_print_segment_callback;
        wparams.new_segment_callback_user_data = &user_data;
      }

      if (wparams.print_progress) {
        wparams.progress_callback = whisper_print_progress_callback;
        wparams.progress_callback_user_data = &user_data;
      }

      // examples for abort mechanism
      // in examples below, we do not abort the processing, but we could if the flag is set to true

      // the callback is called before every encoder run - if it returns false, the processing is aborted
      {
        static bool is_aborted = false; // NOTE: this should be atomic to avoid data race

        wparams.encoder_begin_callback = [](struct whisper_context * /*ctx*/, struct whisper_state * /*state*/,
                                            void *user_data) {
          bool is_aborted = *(bool *) user_data;
          return !is_aborted;
        };
        wparams.encoder_begin_callback_user_data = &is_aborted;
      }

      // the callback is called before every computation - if it returns true, the computation is aborted
      {
        static bool is_aborted = false; // NOTE: this should be atomic to avoid data race

        wparams.abort_callback = [](void *user_data) {
          bool is_aborted = *(bool *) user_data;
          return is_aborted;
        };
        wparams.abort_callback_user_data = &is_aborted;
      }

      if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(), params.n_processors) != 0) {
        fprintf(stderr, "%s: failed to process audio\n", argv[0]);
        const std::string error_resp = "{\"error\":\"failed to process audio\"}";
        res.set_content(error_resp, "application/json");
        whisper_mutex.unlock();
        return;
      }
    }

    // return results to user
    if (params.response_format == text_format) {
      std::string results = output_str(ctx, params, pcmf32s);
      res.set_content(results.c_str(), "text/html");
    }
      // TODO add more output formats
    else {
      std::string results = output_str(ctx, params, pcmf32s);
      json jres = json{
        {"text", results}
      };
      res.set_content(jres.dump(-1, ' ', false, json::error_handler_t::replace),
                      "application/json");
    }

    // return whisper model mutex lock
    whisper_mutex.unlock();
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
    fprintf(stderr, "\ncouldn't bind to server socket: hostname=%s port=%d\n\n",
            sparams.hostname.c_str(), sparams.port);
    return 1;
  }

  // Set the base directory for serving static files
  svr.set_base_dir(sparams.public_path);

  // to make it ctrl+clickable:
  printf("\nwhisper server listening at http://%s:%d\n\n", sparams.hostname.c_str(), sparams.port);

  if (!svr.listen_after_bind()) {
    return 1;
  }

  whisper_print_timings(ctx);
  whisper_free(ctx);

  return 0;
}