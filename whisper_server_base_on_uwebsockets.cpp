#include "nlohmann/json.hpp"
#include "stream_components_params.h"
#include "stream_components_service.h"
#include "stream_components.h"

#include <uwebsockets/App.h>
#include <iostream>
#include <string>
#include <whisper.h>

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

  // WebSocket /paddlespeech/asr/streaming 处理器
  auto ws_streaming_handler = [&whisperService](auto *ws, std::string_view message, uWS::OpCode opCode) {
    if (opCode == uWS::OpCode::TEXT) {
      printf("%s: Received message on /paddlespeech/asr/streaming: %s\n", __func__, std::string(message).c_str());
      // 处理文本消息（假设是 JSON）
      try {
        auto jsonMsg = nlohmann::json::parse(message);
        std::string signal = jsonMsg["signal"];
        // 处理逻辑...
      } catch (const std::exception &e) {
        std::cerr << "JSON 解析错误: " << e.what() << std::endl;
      }
    } else if (opCode == uWS::OpCode::BINARY) {
      // 处理二进制消息（PCM16 数据）
      auto size = message.size();
      printf("%s: Received message size on /paddlespeech/asr/streaming: %zu\n", __func__, size);
      std::vector<int16_t> pcm16(size / 2);
      std::memcpy(pcm16.data(), message.data(), size);

      // 将 PCM16 数据转换为 float
      std::vector<float> pcmf32(pcm16.size());
      std::transform(pcm16.begin(), pcm16.end(), pcmf32.begin(), [](int16_t sample) {
        return static_cast<float>(sample) / 32768.0f; // 将 int16 范围转换为 [-1.0, 1.0]
      });

      // 语音识别处理
      bool isOk = whisperService.process(pcmf32.data(), pcmf32.size());
      printf("isOk:%d\n", isOk);
      const int n_segments = whisper_full_n_segments(whisperService.ctx);
      printf("n_segments:%d\n", n_segments);
      for (int i = 0; i < n_segments; ++i) {
        const char *text = whisper_full_get_segment_text(whisperService.ctx, i);
        const int64_t t0 = whisper_full_get_segment_t0(whisperService.ctx, i);
        const int64_t t1 = whisper_full_get_segment_t1(whisperService.ctx, i);
        printf("%lld-->%lld:%s\n", t0, t1, text);
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
