#include "nlohmann/json.hpp"
#include "stream/stream_components_service.h"
#include "stream/stream_components.h"
#include "common/utils.h"
#include "common/common.h"
#include <uwebsockets/App.h>
#include <iostream>
#include <string>
#include <whisper.h>
#include <sstream>

using namespace stream_components;

bool processAudio(WhisperService service, std::vector<float> pcm32, const whisper_local_stream_params &params);

int main(int argc, char **argv) {
  // Read parameters...
  whisper_local_stream_params params;

  if (!whisper_params_parse(argc, argv, params)) {
    return 1;
  }

  // Compute derived parameters
  params.initialize();
  //output params


  // Check parameters
  if (params.service.language != "auto" && whisper_lang_id(params.service.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.service.language.c_str());
    whisper_print_usage(argc, argv, params);
    exit(0);
  }

  // Instantiate the service
  struct whisper_context_params cparams{};
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
  //Save Audio
  auto ws_save_handler = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
    auto *userData = (std::string *) ws->getUserData();
    // printf("%s: User Data: %s\n", get_current_time().c_str(), userData->c_str());
    thread_local wav_writer wavWriter;
    thread_local std::string filename;

    nlohmann::json response;
    if (opCode == uWS::OpCode::TEXT) {
      // printf("%s: Received message on /streaming/save: %s\n", get_current_time().c_str(),std::string(message).c_str());
      auto jsonMsg = nlohmann::json::parse(message);
      std::string signal = jsonMsg["signal"];
      if (signal == "start") {
        if (jsonMsg["name"].is_string()) {
          filename = jsonMsg["name"];
        } else {
          filename = std::to_string(get_current_time_millis()) + ".wav";
        }
        // 发送服务器准备好的消息
        response = {{"status", "ok"},
                    {"signal", "server_ready"}};
        ws->send(response.dump(), uWS::OpCode::TEXT);
        wavWriter.open(filename, WHISPER_SAMPLE_RATE, 16, 1);
      }
      if (signal == "end") {
        wavWriter.close();
        response = {{"name",   filename},
                    {"signal", signal}};
        ws->send(response.dump(), uWS::OpCode::TEXT);
      }

    } else if (opCode == uWS::OpCode::BINARY) {
      // process binary message（PCM16 data）
      auto size = message.size();
      std::basic_string_view<char, std::char_traits<char>>::const_pointer data = message.data();
      // printf("%s: Received message size on /streaming/save: %zu\n", get_current_time().c_str(), size);
      // add received PCM16 to audio cache
      std::vector<int16_t> pcm16(size / 2);
      std::memcpy(pcm16.data(), data, size);
      //write to file
      wavWriter.write(pcm16.data(), size / 2);
      ws->send(response.dump(), uWS::OpCode::TEXT);
    }
  };

  // WebSocket /paddlespeech/asr/streaming handler
  auto ws_streaming_handler = [&whisperService, &params](auto *ws, std::string_view message, uWS::OpCode opCode) {
    thread_local std::vector<int16_t> audioBuffer; //thread-localized variable
    thread_local wav_writer wavWriter;
    thread_local std::string filename;
    //std::unique_ptr<nlohmann::json> results(new nlohmann::json(nlohmann::json::array()));
    thread_local nlohmann::json final_results;
    auto thread_id = std::this_thread::get_id();
    // std::cout << get_current_time().c_str() << ": Handling a message in thread: " << thread_id << std::endl;
    nlohmann::json response;
    if (opCode == uWS::OpCode::TEXT) {
      // printf("%s: Received message on /paddlespeech/asr/streaming: %s\n", get_current_time().c_str(),std::string(message).c_str());
      // process text message
      try {
        auto jsonMsg = nlohmann::json::parse(message);
        std::string signal = jsonMsg["signal"];
        if (signal == "start") {
          if (jsonMsg["name"].is_string()) {
            filename = jsonMsg["name"];
          } else {
            filename = std::to_string(get_current_time_millis()) + ".wav";
          }
          final_results = nlohmann::json(nlohmann::json::array());
          // 发送服务器准备好的消息
          response = {{"status", "ok"},
                      {"signal", "server_ready"}};
          ws->send(response.dump(), uWS::OpCode::TEXT);
          wavWriter.open(filename, WHISPER_SAMPLE_RATE, 16, 1);
        }
        if (signal == "end") {
          wavWriter.close();
//          nlohmann::json response = {{"name",filename},{"signal", signal}};
          response = {{"name",   filename},
                      {"signal", signal}};

          std::vector<float> pcm32(audioBuffer.size());
          std::transform(audioBuffer.begin(), audioBuffer.end(), pcm32.begin(), [](int16_t sample) {
            return static_cast<float>(sample) / 32768.0f;
          });
          audioBuffer.clear();
          // 如果开启了VAD
          bool isOk;
          if (params.audio.use_vad) {
            // printf("%s: vad: %d \n", get_current_time().c_str(), params.audio.use_vad);
            // TODO: 实现VAD处理，
            //bool containsVoice = vad_simple(audioBuffer, WHISPER_SAMPLE_RATE, 1000, params.audio.vad_thold, params.audio.freq_thold, false);
            isOk = whisperService.process(pcm32.data(), pcm32.size());
          } else {
            // asr
            isOk = whisperService.process(pcm32.data(), pcm32.size());
          }
          if (isOk) {
            final_results = get_result(whisperService.ctx);
            response["result"] = final_results;
          }
          ws->send(response.dump(), uWS::OpCode::TEXT);
        }
        // other process logic...
      } catch (const std::exception &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        auto size = message.size();
      }
    } else if (opCode == uWS::OpCode::BINARY) {
      int size = message.size();
      // process binary message（PCM16 data）
      std::basic_string_view<char, std::char_traits<char>>::const_pointer data = message.data();
      // printf("%s: Received message size on /paddlespeech/asr/streaming: %zu\n", get_current_time().c_str(), size);
      // add received PCM16 to audio cache
      std::vector<int16_t> pcm16(size / 2);

      std::memcpy(pcm16.data(), data, size);
      //write to file
      wavWriter.write(pcm16.data(), size / 2);

      audioBuffer.insert(audioBuffer.end(), pcm16.begin(), pcm16.end());
      unsigned long bufferSize = audioBuffer.size();
      if (bufferSize > 16000 * 10) {
        std::vector<float> pcm32(bufferSize);
        std::transform(audioBuffer.begin(), audioBuffer.end(), pcm32.begin(), [](int16_t sample) {
          return static_cast<float>(sample) / 32768.0f;
        });
        audioBuffer.clear();
        // 如果开启了VAD
        bool isOk;
        if (params.audio.use_vad) {
          // printf("%s: vad: %d \n", get_current_time().c_str(), params.audio.use_vad);
          // TODO: 实现VAD处理，
          //bool containsVoice = vad_simple(audioBuffer, WHISPER_SAMPLE_RATE, 1000, params.audio.vad_thold, params.audio.freq_thold, false);
          isOk = whisperService.process(pcm32.data(), pcm32.size());
        } else {
          // asr
          isOk = whisperService.process(pcm32.data(), pcm32.size());
        }
        if (isOk) {
          final_results = get_result(whisperService.ctx);
          response["result"] = final_results;
        }
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
      //only_save_audio
    .ws<std::string>("/streaming/save", {.open=[](auto *ws) {
      // 初始化用户数据
      auto *userData = (std::string *) ws->getUserData();
      *userData = "Create User Id";  // 设置初始值
    }, .message = ws_save_handler})
      //streaming asr
    .ws<std::string>("/paddlespeech/asr/streaming", {.message = ws_streaming_handler})
      //listen
    .listen(port, started_handler).run();
}

bool processAudio(WhisperService whisperService, std::vector<float> pcm32, const whisper_local_stream_params &params) {
  if (params.audio.use_vad) {
    // printf("%s: vad: %d \n", get_current_time().c_str(), params.audio.use_vad);
    // TODO: 实现VAD处理，
    //bool containsVoice = vad_simple(audioBuffer, WHISPER_SAMPLE_RATE, 1000, params.audio.vad_thold, params.audio.freq_thold, false);
    return whisperService.process(pcm32.data(), pcm32.size());
  } else {
    // asr
    return whisperService.process(pcm32.data(), pcm32.size());
  }
}




