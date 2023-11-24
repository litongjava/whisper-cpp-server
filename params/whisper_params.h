#pragma once

#include <cmath>
#include <fstream>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <cstring>

// output formats
const std::string json_format = "json";
const std::string text_format = "text";
const std::string srt_format = "srt";
const std::string vjson_format = "verbose_json";
const std::string vtt_format = "vtt";

struct whisper_params {
  int32_t n_threads = std::min(4, (int32_t) std::thread::hardware_concurrency());
  int32_t n_processors = 1;
  int32_t offset_t_ms = 0;
  int32_t offset_n = 0;
  int32_t duration_ms = 0;
  int32_t progress_step = 5;
  int32_t max_context = -1;
  int32_t max_len = 0;
  int32_t best_of = 2;
  int32_t beam_size = -1;

  float word_thold = 0.01f;
  float entropy_thold = 2.40f;
  float logprob_thold = -1.00f;
  float userdef_temp = 0.20f;

  bool speed_up = false;
  bool debug_mode = false;
  bool translate = false;
  bool detect_language = false;
  bool diarize = false;
  bool tinydiarize = false;
  bool split_on_word = false;
  bool no_fallback = false;
  bool print_special = false;
  bool print_colors = false;
  bool print_progress = false;
  bool no_timestamps = false;
  bool log_score = false;
  bool use_gpu = true;

  std::string language = "en";
  std::string prompt = "";
  std::string font_path = "/System/Library/Fonts/Supplemental/Courier New Bold.ttf";
  std::string model = "../models/ggml-base.en.bin";

  std::string response_format = json_format;

  // [TDRZ] speaker turn string
  std::string tdrz_speaker_turn = " [SPEAKER_TURN]"; // TODO: set from command line

  std::string openvino_encode_device = "CPU";
  std::string audio_format="wav";
};

struct server_params {
  std::string hostname = "0.0.0.0";
  std::string public_path = "public";
  int32_t port = 8080;
  int32_t read_timeout = 600;
  int32_t write_timeout = 600;
};

void whisper_print_usage(int /*argc*/, char **argv, const whisper_params &params, const server_params &sparams);
bool whisper_params_parse(int argc, char **argv, whisper_params &params, server_params &sparams);