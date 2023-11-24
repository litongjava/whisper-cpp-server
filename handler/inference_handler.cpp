#include <whisper.h>
#include "inference_handler.h"
#include "../common/common.h"
#include "../params/whisper_params.h"
#include "../nlohmann/json.hpp"
#include "../common/utils.h"

using json = nlohmann::json;

struct whisper_print_user_data {
  const whisper_params *params;

  const std::vector<std::vector<float>> *pcmf32s;
  int progress_prev;
};


// Terminal color map. 10 colors grouped in ranges [0.0, 0.1, ..., 0.9]
// Lowest is red, middle is yellow, highest is green.
const std::vector<std::string> k_colors = {
  "\033[38;5;196m", "\033[38;5;202m", "\033[38;5;208m", "\033[38;5;214m", "\033[38;5;220m",
  "\033[38;5;226m", "\033[38;5;190m", "\033[38;5;154m", "\033[38;5;118m", "\033[38;5;82m",
};


int timestamp_to_sample(int64_t t, int n_samples) {
  return std::max(0, std::min((int) n_samples - 1, (int) ((t * WHISPER_SAMPLE_RATE) / 100)));
}

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


void whisper_print_progress_callback(struct whisper_context * /*ctx*/, struct whisper_state * /*state*/, int progress,
                                     void *user_data) {
  int progress_step = ((whisper_print_user_data *) user_data)->params->progress_step;
  int *progress_prev = &(((whisper_print_user_data *) user_data)->progress_prev);
  if (progress >= *progress_prev + progress_step) {
    *progress_prev += progress_step;
    fprintf(stderr, "%s: progress = %3d%%\n", __func__, progress);
  }
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
  if (req.has_file("audio_format")) {
    params.audio_format = req.get_file_value("audio_format").content;
  }
}


void getReqParameters(const Request &request, whisper_params &params);

bool read_audio_file(std::string audio_format, std::string filename, std::vector<float> &pcmf32,
                     std::vector<std::vector<float>> &pcmf32s, bool diarize) {

  // read audio content into pcmf32
  if (audio_format == "mp3") {
    if (!::read_mp3(filename, pcmf32, diarize)) {
      fprintf(stderr, "error: failed to read mp3 file '%s'\n", filename.c_str());
      return false;
    }
  } else if (audio_format == "m4a") {
    if (!::read_m4a(filename, pcmf32, pcmf32s, diarize)) {
      fprintf(stderr, "error: failed to read m4a file '%s'\n", filename.c_str());
      return false;
    }
  } else {
    if (!::read_wav(filename, pcmf32, pcmf32s, diarize)) {
      fprintf(stderr, "error: failed to read WAV file '%s'\n", filename.c_str());
      return false;
    }
  }
  return true;
}

bool run(std::mutex &whisper_mutex, whisper_params &params, whisper_context *ctx, std::string filename,
         const std::vector<std::vector<float>> &pcmf32s, std::vector<float> pcmf32) {
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

    // aquire whisper model mutex lock
    whisper_mutex.lock();
    if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(), params.n_processors) != 0) {
      fprintf(stderr, "%s: failed to process audio\n", filename.c_str());
      whisper_mutex.unlock();
      return false;
    }
    whisper_mutex.unlock();
    return true;
  }
}


void handleInference(const Request &request, Response &response, std::mutex &whisper_mutex, whisper_params &params,
                     whisper_context *ctx, char *arg_audio_file) {
  // first check user requested fields of the request
  if (!request.has_file("file")) {
    fprintf(stderr, "error: no 'file' field in the request\n");
    json jres = json{
      {"code", -1},
      {"msg",  "no 'file' field in the request"}
    };
    auto json_string = jres.dump(-1, ' ', false, json::error_handler_t::replace);
    response.set_content(json_string, "application/json");
    return;
  }
  auto audio_file = request.get_file_value("file");
  std::string filename{audio_file.filename};
  printf("%s: Received filename: %s \n", get_current_time().c_str(), filename.c_str());
  // check non-required fields
  getReqParameters(request, params);
  printf("%s: audio_format:%s \n", get_current_time().c_str(), params.audio_format.c_str());

  // audio arrays
  std::vector<float> pcmf32;               // mono-channel F32 PCM
  std::vector<std::vector<float>> pcmf32s; // stereo-channel F32 PCM

  // write file to temporary file
  std::ofstream temp_file{filename, std::ios::binary};
  temp_file << audio_file.content;

  bool isOK = read_audio_file(params.audio_format, filename, pcmf32, pcmf32s, params.diarize);
  if (!isOK) {
    json json_obj = {
      {"code", -1},
      {"msg",  "error: failed to read WAV file "}
    };
    auto json_string = json_obj.dump(-1, ' ', false, json::error_handler_t::replace);
    response.set_content(json_string, "application/json");
    return;
  }

  // remove temp file
  std::remove(filename.c_str());

  printf("Successfully loaded %s\n", filename.c_str());

  bool isOk = run(whisper_mutex, params, ctx, filename, pcmf32s, pcmf32);
  if (isOk) {
    // return results to user
    if (params.response_format == text_format) {
      std::string results = output_str(ctx, params, pcmf32s);
      response.set_content(results.c_str(), "text/html");
    }
      // TODO add more output formats
    else {
      auto results = get_result(ctx);
      json jres = json{
        {"code", 0},
        {"data", results}
      };
      response.set_content(jres.dump(-1, ' ', false, json::error_handler_t::replace),
                           "application/json");
    }
  } else {
    json jres = json{
      {"code", -1},
      {"msg",  "run error"}
    };
    auto json_string = jres.dump(-1, ' ', false, json::error_handler_t::replace);
    response.set_content(json_string, "application/json");
  }
}
