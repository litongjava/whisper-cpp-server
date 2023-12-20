#include "stream_components_service.h"

using namespace stream_components;

// -- WhisperService --

WhisperService::WhisperService(const struct service_params &sparams,
                               const struct audio_params &aparams,
                               const struct whisper_context_params &cparams)
    : sparams(sparams),
      aparams(aparams),
      ctx(whisper_init_from_file_with_params(sparams.model.c_str(), cparams))
{
  // print system information
  {
    fprintf(stderr, "\n");
    fprintf(stderr, "system_info: n_threads = %d / %d | %s\n",
            sparams.n_threads * sparams.n_processors, std::thread::hardware_concurrency(),
            whisper_print_system_info());
  }
  {
    fprintf(stderr, "\n");
    if (!whisper_is_multilingual(ctx))
    {
      if (sparams.language != "en" || sparams.translate)
      {
        this->sparams.language = "en";
        this->sparams.translate = false;
        fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n",
                __func__);
      }
    }
    fprintf(stderr, "%s: serving with %d threads, %d processors, lang = %s, task = %s, timestamps = %d ...\n",
            __func__,
            sparams.n_threads,
            sparams.n_processors,
            sparams.language.c_str(),
            sparams.translate ? "translate" : "transcribe",
            sparams.no_timestamps ? 0 : 1);

    //     if (!audio_params.use_vad) {
    //         fprintf(stderr, "%s: n_new_line = %d, no_context = %d\n", __func__, n_new_line, sparams.no_context);
    //     }

    fprintf(stderr, "\n");
  }
}

WhisperService::~WhisperService()
{
  whisper_print_timings(ctx);
  whisper_free(ctx);
}

bool WhisperService::process(const float *samples, int sample_count)
{
  whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

  wparams.print_progress = false;
  wparams.print_realtime = false;
  wparams.print_timestamps = false;
  wparams.print_special = true;
  wparams.max_tokens = 0;
  wparams.token_timestamps = true;

  wparams.translate = sparams.translate;
  wparams.single_segment = !aparams.use_vad;
  wparams.language = sparams.language.c_str();
  wparams.n_threads = sparams.n_threads;

  wparams.audio_ctx = aparams.audio_ctx;
  wparams.speed_up = sparams.speed_up;

  wparams.tdrz_enable = sparams.tinydiarize; // [TDRZ]

  // disable temperature fallback
  // wparams.temperature_inc  = -1.0f;
  wparams.temperature_inc = sparams.no_fallback ? 0.0f : wparams.temperature_inc;

  wparams.prompt_tokens = sparams.no_context ? nullptr : prompt_tokens.data();
  wparams.prompt_n_tokens = sparams.no_context ? 0 : prompt_tokens.size();

  // *** Run the actual inference!!! ***
  //  if (whisper_full(ctx, wparams, samples, sample_count) != 0) {
  //    return false;
  //  }
  // whisper_full_parallel
  if (whisper_full_parallel(ctx, wparams, samples, sample_count, sparams.n_processors) != 0)
  {
    // error:ggml_metal_get_buffer: error: buffer is nil
    return false;
  }

  // Now sure whether n_iter and n_new_line should have ever been there...
  // *** SUSPICIOUS what happens by removing them? Are they essential?
  // if (!use_vad && (n_iter % n_new_line) == 0) {
  //  if (!audio_params.use_vad) {
  // printf("\n");

  // keep part of the audio for next iteration to try to mitigate word boundary issues
  // *** I don't know if we need this...
  // pcmf32_old = std::vector<float>(pcmf32.end() - n_samples_keep, pcmf32.end());

  // Add tokens of the last full length segment as the prompt
  //    if (!sparams.no_context) {
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