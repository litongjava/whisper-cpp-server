#include <iostream>
#include <vector>
#include <cstdint>
#include <whisper.h>

#include "../stream/stream_components_service.h"
#include "../stream/stream_components.h"
#include "../common/utils.h"
#include "../common/common.h"
#include <speex/speex_preprocess.h>

using namespace stream_components;


int main() {
  std::string wav_file_path = "../samples/jfk.wav";  // 替换为您的 WAV 文件路径
  // audio arrays
  std::vector<float> pcmf32;               // mono-channel F32 PCM
  std::vector<std::vector<float>> pcmf32s; // stereo-channel F32 PCM
  ::read_wav(wav_file_path, pcmf32, pcmf32s, false);

  printf("size of samples %lu\n", pcmf32.size());


  whisper_local_stream_params params;
  struct whisper_context_params cparams{};
  cparams.use_gpu = params.service.use_gpu;
  //Instantiate the service
  stream_components::WhisperService whisperService(params.service, params.audio, cparams);

  //Simulate websokcet by adding 1500 data each time.
  std::vector<float> audio_buffer;
  int chunk_size = 160; // 适用于 16 kHz 采样率的 100 毫秒帧
  SpeexPreprocessState *st = speex_preprocess_state_init(chunk_size, WHISPER_SAMPLE_RATE);
  int vad = 1;
  speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_VAD, &vad);

  bool last_is_speech = false;
  // 处理音频帧
  for (size_t i = 0; i < pcmf32.size(); i += chunk_size) {
    spx_int16_t frame[chunk_size];
    for (int j = 0; j < chunk_size; ++j) {
      if (i + j < pcmf32.size()) {
        frame[j] = (spx_int16_t)(pcmf32[i + j] * 32768);
      } else {
        frame[j] = 0; // 对于超出范围的部分填充 0
      }
    }
    int is_speech = speex_preprocess_run(st, frame);

    // 将当前帧添加到 audio_buffer
    audio_buffer.insert(audio_buffer.end(), pcmf32.begin() + i, pcmf32.begin() + std::min(i + chunk_size, pcmf32.size()));
    printf("is_speech %d \n",is_speech);
    if (!is_speech && last_is_speech) {
      bool b = whisperService.process(pcmf32.data(), pcmf32.size());
      const nlohmann::json &json_array = get_result(whisperService.ctx);
      const std::basic_string<char, std::char_traits<char>, std::allocator<char>> &string = json_array.dump();
      printf("%s\n",string.c_str());
      return 0;
      audio_buffer.clear();
    }

    last_is_speech = is_speech != 0;
  }

  speex_preprocess_state_destroy(st);
}