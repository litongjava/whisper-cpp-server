#include "nlohmann/json.hpp"
#include "stream_components_params.h"
#include "stream_components_service.h"
#include "stream_components.h"

#include <uwebsockets/App.h>
#include <iostream>
#include <string>
#include <whisper.h>
#include <sstream>

using namespace stream_components;

std::string get_current_time() {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  std::stringstream current_time_ss;
  current_time_ss << std::put_time(std::localtime(&now_c), "%y-%m-%d %H:%M:%S")
                  << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

  std::string current_time = current_time_ss.str();
  return current_time;
}

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

  // Instantiate the microphone input
  // stream_components::LocalSDLMicrophone microphone(params.audio);

  // Instantiate the service
  struct whisper_context_params cparams;
  cparams.use_gpu = params.service.use_gpu;
  stream_components::WhisperService whisperService(params.service, params.audio, cparams);

  // Print the 'header'...
  //WhisperStreamOutput::to_json(std::cout, params.service, whisperService.ctx);

  const int port = 8090;

  // 开始处理器
  auto started_handler = [](auto *token) {
    if (token) {
      std::cout << "Server started on port " << port << std::endl;
    } else {
      std::cerr << "Failed to start server" << std::endl;
    }
  };

  // HTTP GET /hello 处理器
  auto hello_action = [](auto *res, auto *req) {
    res->end("Hello World!");
  };

  // WebSocket /echo 处理器
  auto ws_echo_handler = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
    ws->send(message, opCode);
  };

  // WebSocket /paddlespeech/asr/streaming handler
  std::vector<float> audioBuffer; // global audio data buffer点击并应用
  auto ws_streaming_handler = [&whisperService, &audioBuffer](auto *ws, std::string_view message, uWS::OpCode opCode) {
    if (opCode == uWS::OpCode::TEXT) {
      printf("%s: Received message on /paddlespeech/asr/streaming: %s\n", get_current_time().c_str(),
             std::string(message).c_str());
      // process text message
      try {
        auto jsonMsg = nlohmann::json::parse(message);
        std::string signal = jsonMsg["signal"];
        if (signal == "start") {
          // 发送服务器准备好的消息
          nlohmann::json response = {{"status", "ok"},
                                     {"signal", "server_ready"}};
          ws->send(response.dump(), uWS::OpCode::TEXT);
        }
        // other process logic...
      } catch (const std::exception &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
      }
    } else if (opCode == uWS::OpCode::BINARY) {
      // process binary message（PCM16 data）
      auto size = message.size();
      printf("%s: Received message size on /paddlespeech/asr/streaming: %zu\n", get_current_time().c_str(), size);
      // 将接收到的 PCM16 数据追加到音频缓存
      std::vector<int16_t> pcm16(size / 2);
      std::memcpy(pcm16.data(), message.data(), size);

      std::transform(pcm16.begin(), pcm16.end(), std::back_inserter(audioBuffer), [](int16_t sample) {
        return static_cast<float>(sample) / 32768.0f; // convert to  [-1.0, 1.0] float
      });

      // 语音识别处理
      bool isOk = whisperService.process(audioBuffer.data(), audioBuffer.size());
      printf("%s: isOk:%d\n", get_current_time().c_str(), isOk);
      if (isOk) {
        nlohmann::json response;
        nlohmann::json results = nlohmann::json::array(); // create JSON Array

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
        ws->send(response.dump(), uWS::OpCode::TEXT);
      }
    }
  };

  // 设置 uWebSockets 应用
  uWS::App()
    .get("/hello", hello_action)
    .ws<std::string>("/echo", {.message = ws_echo_handler})
    .ws<std::string>("/paddlespeech/asr/streaming", {.message = ws_streaming_handler})
    .listen(port, started_handler).run();
}
