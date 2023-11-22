#include "nlohmann/json.hpp"
#include "stream/stream_components_service.h"
#include "stream/stream_components.h"
#include "utils/utils.h"
#include "common/common.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <string>
#include <whisper.h>
#include <sstream>

using namespace stream_components;

int main(int argc, char **argv) {
  // Read parameters...
  whisper_local_stream_params params;

  if (whisper_params_parse(argc, argv, params) == false) {
    return 1;
  }

  // Compute derived parameters
  params.initialize();
  //output params
  printf("vad:%d\n", params.audio.use_vad);

  // Check parameters
  if (params.service.language != "auto" && whisper_lang_id(params.service.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.service.language.c_str());
    whisper_print_usage(argc, argv, params);
    exit(0);
  }

  // Instantiate the service
  struct whisper_context_params cparams;
  cparams.use_gpu = params.service.use_gpu;
  stream_components::WhisperService whisperService(params.service, params.audio, cparams);

  const int port = 8090;

  // started handler
  auto started_handler = [](auto *token) {
    if (token) {
      std::cout << "Server started on port " << port << std::endl;
    } else {
      std::cerr << "Failed to start server" << std::endl;
    }
  };

  // HTTP GET /hello handler
  auto hello_action = [](auto *res, auto *req) {
    res->end("Hello World!");
  };

  // WebSocket /echo handler
  auto ws_echo_handler = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
    ws->send(message, opCode);
  };

  // WebSocket /paddlespeech/asr/streaming handler
  auto ws_streaming_handler = [&whisperService](auto *ws, std::string_view message, uWS::OpCode opCode) {
    thread_local std::vector<float> audioBuffer; //thread-localized variable
    thread_local wav_writer wavWriter;
    std::string filename;
    //std::unique_ptr<nlohmann::json> results(new nlohmann::json(nlohmann::json::array()));
    nlohmann::json results = nlohmann::json(nlohmann::json::array());

    if (opCode == uWS::OpCode::TEXT) {
      printf("%s: Received message on /paddlespeech/asr/streaming: %s\n", get_current_time().c_str(),
             std::string(message).c_str());
      // process text message
      try {
        auto jsonMsg = nlohmann::json::parse(message);
        if (jsonMsg["name"].is_string()) {
          filename = jsonMsg["name"];
        } else {
          filename = std::to_string(get_current_time_millis()) + ".wav";
        }
        std::string signal = jsonMsg["signal"];
        if (signal == "start") {
          // 发送服务器准备好的消息
          nlohmann::json response = {{"status", "ok"},
                                     {"signal", "server_ready"}};
          ws->send(response.dump(), uWS::OpCode::TEXT);
          wavWriter.open(filename, WHISPER_SAMPLE_RATE, 16, 1);
        }
        if (signal == "end") {
          wavWriter.close();
//          nlohmann::json response = {{"name",filename},{"signal", signal}};
          nlohmann::json response = {{"name",   filename},
                                     {"signal", signal},
                                     {"result", results}};
          ws->send(response.dump(), uWS::OpCode::TEXT);
        }
        // other process logic...
      } catch (const std::exception &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
      }
    } else if (opCode == uWS::OpCode::BINARY) {
      nlohmann::json response;

      // process binary message（PCM16 data）
      auto size = message.size();

      printf("%s: Received message size on /paddlespeech/asr/streaming: %zu\n", get_current_time().c_str(), size);
      // add received PCM16 to audio cache
      std::vector<int16_t> pcm16(size / 2);
      std::basic_string_view<char, std::char_traits<char>>::const_pointer data = message.data();
      std::memcpy(pcm16.data(), data, size);

      std::vector<float> temp(size / 2);
      std::transform(pcm16.begin(), pcm16.end(), temp.begin(), [](int16_t sample) {
        return static_cast<float>(sample) / 32768.0f;
      });
      //write to file
      wavWriter.write(temp.data(), size / 2);
      audioBuffer.insert(audioBuffer.end(), temp.begin(), temp.end());
      // asr
      bool isOk = whisperService.process(audioBuffer.data(), audioBuffer.size());
      if (isOk) {
        const int n_segments = whisper_full_n_segments(whisperService.ctx);
        for (int i = 0; i < n_segments; ++i) {
          nlohmann::json segment;
          int64_t t0 = whisper_full_get_segment_t0(whisperService.ctx, i);
          int64_t t1 = whisper_full_get_segment_t1(whisperService.ctx, i);
          const char *sentence = whisper_full_get_segment_text(whisperService.ctx, i);
          auto result = std::to_string(t0) + "-->" + std::to_string(t1) + ":" + sentence + "\n";
          printf("%s: result:%s\n", get_current_time().c_str(), result.c_str());
          segment["t0"] = t0;
          segment["t1"] = t1;
          segment["sentence"] = sentence;
          results.push_back(segment);
        }
        response["result"] = results;
      }

      ws->send(response.dump(), uWS::OpCode::TEXT);
    }
  };

  // config uWebSockets app
  uWS::App()
    //hello
    .get("/hello", hello_action)
      //echo
    .ws<std::string>("/echo", {.message = ws_echo_handler})
      //streaming
    .ws<std::string>("/paddlespeech/asr/streaming", {.message = ws_streaming_handler})
      //listen
    .listen(port, started_handler).run();
}
