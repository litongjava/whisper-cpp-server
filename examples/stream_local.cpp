#include <iostream>
#include "../stream/stream_components_audio.h"
#include "../stream/stream_components_params.h"
#include "../stream/stream_components_output.h"
#include "../stream/stream_components_service.h"
#include "../stream/stream_components.h"
//#include "json.hpp"

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
  stream_components::LocalSDLMicrophone microphone(params.audio);

  // Instantiate the service
  struct whisper_context_params cparams;
  cparams.use_gpu = params.service.use_gpu;
  stream_components::WhisperService whisperService(params.service, params.audio, cparams);

  // Print the 'header'...
  WhisperStreamOutput::to_json(std::cout, params.service, whisperService.ctx);

  // Run until Ctrl + C
  bool is_running = true;
  while (is_running) {

    // handle Ctrl + C
    is_running = sdl_poll_events();
    if (!is_running) {
      break;
    }

    // get next microphone section
    auto pcmf32 = microphone.get_next();

    // process
    bool isOk = whisperService.process(pcmf32.data(), pcmf32.size());
    printf("isOk:%d\n", isOk);
    // get the whisper output
    if (isOk) {
//      WhisperOutputPtr outputPtr = std::make_shared<WhisperStreamOutput>(whisperService.ctx, params.service);
//      // write the output as json to stdout (for this example)
//      if (outputPtr) {
//        outputPtr->transcription_to_json(std::cout);
//      }
      const int n_segments = whisper_full_n_segments(whisperService.ctx);
      printf("n_segments:%d\n", n_segments);
      for (int i = 0; i < n_segments; ++i) {
        const char *text = whisper_full_get_segment_text(whisperService.ctx, i);
        const int64_t t0 = whisper_full_get_segment_t0(whisperService.ctx, i);
        const int64_t t1 = whisper_full_get_segment_t1(whisperService.ctx, i);
        printf("%lld-->%lld:%s\n", t0, t1, text);
      }
    }

  }
  std::cout << "EXITED MAIN LOOP" << std::endl;
  return 0;
}