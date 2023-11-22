//
// Created by Ping Lee on 2023/11/21.
//

#include "utils.h"
#include <string>
#include <sstream>
#include <iomanip>

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