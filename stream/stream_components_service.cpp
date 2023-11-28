#include "stream_components_service.h"

using namespace stream_components;


// -- WhisperService --

WhisperService::WhisperService(const struct service_params &service_params,
                               const struct audio_params &audio_params,
                               const struct whisper_context_params &cparams)
  : service_params(service_params),
    audio_params(audio_params),
    ctx(whisper_init_from_file_with_params(service_params.model.c_str(), cparams)) {
  {
    fprintf(stderr, "\n");
    if (!whisper_is_multilingual(ctx)) {
      if (service_params.language != "en" || service_params.translate) {
        this->service_params.language = "en";
        this->service_params.translate = false;
        fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n",
                __func__);
      }
    }
    fprintf(stderr, "%s: serving with %d threads, lang = %s, task = %s, timestamps = %d ...\n",
            __func__,
            service_params.n_threads,
            service_params.language.c_str(),
            service_params.translate ? "translate" : "transcribe",
            service_params.no_timestamps ? 0 : 1);

    // if (!params.use_vad) {
    //     fprintf(stderr, "%s: n_new_line = %d, no_context = %d\n", __func__, n_new_line, params.no_context);
    // }

    fprintf(stderr, "\n");
  }
}

WhisperService::~WhisperService() {
  whisper_print_timings(ctx);
  whisper_free(ctx);
}

bool WhisperService::process(const float *samples, int sample_count) {
  whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

  wparams.print_progress = false;
  wparams.print_realtime = false;
  wparams.print_timestamps = false;
  wparams.print_special = true;
  wparams.max_tokens = 0;
  wparams.token_timestamps = true;

  wparams.translate = service_params.translate;
  wparams.single_segment = !audio_params.use_vad;
  wparams.language = service_params.language.c_str();
  wparams.n_threads = service_params.n_threads;

  wparams.audio_ctx = audio_params.audio_ctx;
  wparams.speed_up = service_params.speed_up;

  wparams.tdrz_enable = service_params.tinydiarize; // [TDRZ]

  // disable temperature fallback
  //wparams.temperature_inc  = -1.0f;
  wparams.temperature_inc = service_params.no_fallback ? 0.0f : wparams.temperature_inc;

  wparams.prompt_tokens = service_params.no_context ? nullptr : prompt_tokens.data();
  wparams.prompt_n_tokens = service_params.no_context ? 0 : prompt_tokens.size();

  // *** Run the actual inference!!! ***
//  if (whisper_full(ctx, wparams, samples, sample_count) != 0) {
//    return false;
//  }
  //whisper_full_parallel
  if (whisper_full_parallel(ctx, wparams, samples, sample_count,service_params.n_processors) != 0) {
    //error:ggml_metal_get_buffer: error: buffer is nil
    return false;
  }

// Now sure whether n_iter and n_new_line should have ever been there...
// *** SUSPICIOUS what happens by removing them? Are they essential?
//if (!use_vad && (n_iter % n_new_line) == 0) {
//  if (!audio_params.use_vad) {
//printf("\n");

// keep part of the audio for next iteration to try to mitigate word boundary issues
// *** I don't know if we need this...
//pcmf32_old = std::vector<float>(pcmf32.end() - n_samples_keep, pcmf32.end());

// Add tokens of the last full length segment as the prompt
//    if (!service_params.no_context) {
//      prompt_tokens.
//
//        clear();
//
//      const int n_segments = whisper_full_n_segments(ctx);
//      for (
//        int i = 0;
//        i < n_segments;
//        ++i) {
//        const int token_count = whisper_full_n_tokens(ctx, i);
//        for (
//          int j = 0;
//          j < token_count;
//          ++j) {
//          prompt_tokens.
//            push_back(whisper_full_get_token_id(ctx, i, j)
//          );
//        }
//      }
//    }
//  }
  return true;
}