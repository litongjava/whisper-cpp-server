#include <uwebsockets/App.h>
#include <iostream>
#include <string>
#include <whisper.h>
#include <sstream>
#include <speex/speex_preprocess.h>

#include "nlohmann/json.hpp"
#include "stream/stream_components_service.h"
#include "stream/stream_components.h"
#include "common/utils.h"
#include "common/common.h"
#include "handler/hello_handler.h"
#include "handler/echo_handler.h"

using namespace stream_components;

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
  std::mutex whisper_mutex;


  // started handler
  auto started_handler = [](auto *token) {
    if (token) {
      std::cout << "Server started on port " << port << std::endl;
    } else {
      std::cerr << "Failed to start server" << std::endl;
    }
  };


  //Save Audio
  auto ws_save_handler = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
    auto *userData = (std::string *) ws->getUserData();
    // printf("%s: User Data: %s\n", get_current_time().c_str(), userData->c_str());
    thread_local wav_writer wavWriter;
    thread_local std::string filename;
    uint16_t bitsPerSample;

    nlohmann::json response;
    if (opCode == uWS::OpCode::TEXT) {
      printf("%s: Received TEXT message on /paddlespeech/streaming/save: %s\n", get_current_time().c_str(),
             std::string(message).c_str());
      auto jsonMsg = nlohmann::json::parse(message);
      std::string signal = jsonMsg["signal"];
      if (signal == "start") {
        if (jsonMsg["name"].is_string()) {
          filename = jsonMsg["name"];
        } else {
          filename = std::to_string(get_current_time_millis()) + ".wav";
        }
        uint32_t sampleRate = WHISPER_SAMPLE_RATE;
        if (jsonMsg["sampleRate"].is_number_integer()) {
          sampleRate = jsonMsg["sampleRate"].get<uint32_t>();
        }

        if (jsonMsg["bitsPerSample"].is_number_integer()) {
          bitsPerSample = jsonMsg["bitsPerSample"].get<uint16_t>();
        } else {
          bitsPerSample = 16;
        }

        uint16_t channels = 1;
        if (jsonMsg["channels"].is_number_integer()) {
          channels = jsonMsg["channels"].get<uint16_t>();
        }

        // 发送服务器准备好的消息
        response = {{"status", "ok"},
                    {"signal", "server_ready"}};
        printf("%s: Wav info:filename:%s, sampleRate:%d, bitsPerSample:%d, channels:%d \n", get_current_time().c_str(),
               filename.c_str(), sampleRate, bitsPerSample, channels);
        ws->send(response.dump(), uWS::OpCode::TEXT);
        wavWriter.open(filename, sampleRate, bitsPerSample, channels);
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
      printf("%s: Received BINARY message size on /paddlespeech/streaming/save: %zu\n", get_current_time().c_str(),
             size);
      if (bitsPerSample == 32) {
        // Make sure the data size is an integer multiple of the float size.
        if (size % sizeof(float) != 0) {
          std::cerr << "Received data size is not a multiple of sizeof(float)" << std::endl;
          return;
        }

        size_t numFloats = size / sizeof(float);
        std::vector<float> pcmf32(numFloats);
        std::memcpy(pcmf32.data(), data, size);

        // Convert float to int16_t
        std::vector<int16_t> pcm16(numFloats);
        for (size_t i = 0; i < numFloats; ++i) {
          // Converts float samples in the range -1.0 to 1.0 to int16_t in the range -32768 to 32767.
          float sample = pcmf32[i];
          int16_t convertedSample = static_cast<int16_t>(std::max(-1.0f, std::min(1.0f, sample)) * 32767.0f);
          pcm16[i] = convertedSample;
        }

        // write to file
        wavWriter.write(pcm16.data(), pcm16.size());
      } else {
        // add received PCM16 to audio cache
        std::vector<int16_t> pcm16(size / 2);
        std::memcpy(pcm16.data(), data, size);
        //write to file
        wavWriter.write(pcm16.data(), size / 2);
      }
      // Send the message prepared by the server
      response = {{"status", "ok"}};
      ws->send(response.dump(), uWS::OpCode::TEXT);
    }
  };

  // WebSocket /paddlespeech/asr/streaming handler
  auto ws_streaming_handler = [&whisperService, &params, &whisper_mutex](auto *ws, std::string_view message,
                                                                         uWS::OpCode opCode) {
    thread_local std::vector<float> audioBuffer; //thread-localized variable
    thread_local wav_writer wavWriter;
    thread_local std::string filename;
    thread_local bool last_is_speech = false;
    thread_local int chunk_size = 160; // 10 ms frame for 16 kHz sampling rate
    thread_local SpeexPreprocessState *state;

    //std::unique_ptr<nlohmann::json> results(new nlohmann::json(nlohmann::json::array()));
    thread_local nlohmann::json final_results;
    // auto thread_id = std::this_thread::get_id();
    // std::cout << get_current_time().c_str() << ": Handling a message in thread: " << thread_id << std::endl;
    nlohmann::json response;
    if (opCode == uWS::OpCode::TEXT) {
      // process text message
      printf("%s: Received message on /paddlespeech/asr/streaming: %s\n", get_current_time().c_str(),
             std::string(message).c_str());

      try {
        auto jsonMsg = nlohmann::json::parse(message);
        std::string signal = jsonMsg["signal"];
        if (signal == "start") {
          printf("%s start\n", get_current_time().c_str());

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
          state = speex_preprocess_state_init(chunk_size, WHISPER_SAMPLE_RATE);
          int vad = 1;
          speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_VAD, &vad);
          //1. 提高 VAD 灵敏度：
          int prob_start = 80;  // 范围从 0 到 100
          int prob_continue = 65;  // 范围从 0 到 100
          speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_PROB_START, &prob_start);
          speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &prob_continue);

          //2. 噪音抑制：
          int noise_suppress = -30; // dB, 可以尝试 -30 到 -15
          speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noise_suppress);

          //3. 回声消除:
          // SpeexEchoState *echo_state = speex_echo_state_init(chunk_size, echo_tail);
          // speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state);

          //4. 自动增益控制（AGC）：
          // int agc = 1; // 开启 AGC
          // speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_AGC, &agc);


        }
        if (signal == "recognize") {
          printf("%s:buffer size:%lu\n", get_current_time().c_str(), audioBuffer.size());
          bool isOk = whisperService.process(audioBuffer.data(), audioBuffer.size());
          audioBuffer.clear();
          if (isOk) {
            final_results = get_result(whisperService.ctx);
            response["result"] = final_results;
          }
          response["status"] = "ok";
          ws->send(response.dump(), uWS::OpCode::TEXT);
        }


        if (signal == "end") {
//          nlohmann::json response = {{"name",filename},{"signal", signal}};
          response = {{"name",   filename},
                      {"signal", signal}};
          printf("%s:buffer size:%lu\n", get_current_time().c_str(), audioBuffer.size());
          bool isOk = whisperService.process(audioBuffer.data(), audioBuffer.size());
          audioBuffer.clear();
          if (isOk) {
            final_results = get_result(whisperService.ctx);
            response["result"] = final_results;
          }
          const std::basic_string<char, std::char_traits<char>, std::allocator<char>> &string = response.dump();
          printf("%s %s\n", get_current_time().c_str(), string.c_str());
          ws->send(string, uWS::OpCode::TEXT);
          wavWriter.close();
          speex_preprocess_state_destroy(state);
          printf("%s End\n", get_current_time().c_str());
        }
        // other process logic...
      } catch (const std::exception &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        auto size = message.size();
      }
    } else if (opCode == uWS::OpCode::BINARY) {
      auto handler_binary_start = std::chrono::high_resolution_clock::now();

      // process binary message（PCM16 data）
      auto size = message.size();
      std::basic_string_view<char, std::char_traits<char>>::const_pointer data = message.data();
//      printf("%s: Received message size on /paddlespeech/asr/streaming: %zu\n", get_current_time().c_str(), size);
      // add received PCM16 to audio cache
      std::vector<int16_t> pcm16(size / 2);
      std::memcpy(pcm16.data(), data, size);
      //write to file
      wavWriter.write(pcm16.data(), size / 2);
      //convert flost
      std::vector<float> temp(size / 2);
      std::transform(pcm16.begin(), pcm16.end(), temp.begin(), [](int16_t sample) {
        return static_cast<float>(sample) / 32768.0f;
      });
      //insert to audio_buffer
      audioBuffer.insert(audioBuffer.end(), temp.begin(), temp.end());

      // printf("%s:buffer size:% ld\n",get_current_time().c_str(),audioBuffer.size());
      // 如果开启了VAD
      bool isOk;
      // printf("%s: use_vad: %d\n", get_current_time().c_str(), params.audio.use_vad);
      if (params.audio.use_vad) {
        whisper_mutex.lock();
        for (size_t i = 0; i < pcm16.size(); i += chunk_size) {
          spx_int16_t frame[chunk_size];
          for (int j = 0; j < chunk_size; ++j) {
            if (i + j < pcm16.size()) {
              frame[j] = (spx_int16_t) (pcm16[i + j]);
            } else {
              frame[j] = 0; // 对于超出范围的部分填充 0
            }
          }
          int is_speech = speex_preprocess_run(state, frame);
          // printf("%s: is_active: %d,is_last_active %d\n", get_current_time().c_str(), is_speech, last_is_speech);
          if (!is_speech && last_is_speech) {
            size_t sampleCount = audioBuffer.size();
            double durationInSeconds = static_cast<double>(sampleCount) / WHISPER_SAMPLE_RATE;
            auto process_start = std::chrono::high_resolution_clock::now();

            isOk = whisperService.process(audioBuffer.data(), sampleCount);

            auto process_stop = std::chrono::high_resolution_clock::now();
            auto process_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_stop - process_start);

            printf(
              "process execution time: %lld milliseconds audioBuffer size: %zu elements Audio Duration: %f seconds\n",
              process_duration.count(), sampleCount, durationInSeconds);
            audioBuffer.clear();
            last_is_speech = is_speech;
            break;
          } else {
            last_is_speech = is_speech;
          }


        }
        whisper_mutex.unlock();
      } else {
        // asr
        whisper_mutex.lock();
        size_t sampleCount = audioBuffer.size();
        double durationInSeconds = static_cast<double>(sampleCount) / WHISPER_SAMPLE_RATE;
        auto process_start = std::chrono::high_resolution_clock::now();

        isOk = whisperService.process(audioBuffer.data(), sampleCount);
        auto process_stop = std::chrono::high_resolution_clock::now();
        auto process_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_stop - process_start);

        printf("Execution Time: %lld milliseconds audioBuffer size: %zu elements Audio Duration: %f seconds\n",
               process_duration.count(), sampleCount, durationInSeconds);
        whisper_mutex.unlock();
      }
      // printf("%s: is_ok: %d \n", get_current_time().c_str(), isOk);
      if (isOk) {
//        final_results = get_result(whisperService.ctx);
        final_results = nlohmann::json(nlohmann::json::array());
        const int n_segments = whisper_full_n_segments(whisperService.ctx);
        for (int i = 0; i < n_segments; ++i) {
          nlohmann::json segment_json_object;
          int64_t t0 = whisper_full_get_segment_t0(whisperService.ctx, i);
          int64_t t1 = whisper_full_get_segment_t1(whisperService.ctx, i);
          const char *sentence = whisper_full_get_segment_text(whisperService.ctx, i);
          auto result = std::to_string(t0) + "-->" + std::to_string(t1) + ":" + sentence + "\n";
          printf("result:%s", result.c_str());
          segment_json_object["t0"] = to_timestamp(t0);
          segment_json_object["t1"] = to_timestamp(t1);
          segment_json_object["sentence"] = sentence;
          final_results.push_back(segment_json_object);
        }
        response["result"] = final_results;
        auto handler_binary_stop = std::chrono::high_resolution_clock::now();
        auto handler_binary_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
          handler_binary_stop - handler_binary_start);

        printf("Handler binary execution time: %lld milliseconds\n\n",
               handler_binary_duration.count());
      }
      response["status"] = "ok";
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
    .ws<std::string>("/paddlespeech/streaming/save", {.open=[](auto *ws) {
      // init user data
      auto *userData = (std::string *) ws->getUserData();
      *userData = "Create User Id";  // set user data
    }, .message = ws_save_handler})
      //streaming asr
    .ws<std::string>("/paddlespeech/asr/streaming", {.message = ws_streaming_handler})
      //listen
    .listen(port, started_handler).run();
}