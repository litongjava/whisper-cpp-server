//
// Created by Ping Lee on 2023/11/21.
//

#include "utils.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <whisper.h>
#include "../nlohmann/json.hpp"


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

long get_current_time_millis(){
  auto start = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
}

nlohmann::json get_result(whisper_context *ctx) {
  nlohmann::json results = nlohmann::json(nlohmann::json::array());
  const int n_segments = whisper_full_n_segments(ctx);
  for (int i = 0; i < n_segments; ++i) {
    nlohmann::json segment;
    int64_t t0 = whisper_full_get_segment_t0(ctx, i);
    int64_t t1 = whisper_full_get_segment_t1(ctx, i);
    const char *sentence = whisper_full_get_segment_text(ctx, i);
    auto result = std::to_string(t0) + "-->" + std::to_string(t1) + ":" + sentence + "\n";
    printf("%s: result:%s\n", get_current_time().c_str(), result.c_str());
    segment["t0"] = t0;
    segment["t1"] = t1;
    segment["sentence"] = sentence;
    results.push_back(segment);
  }
  return results;
}

