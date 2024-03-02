#pragma once

#include <iostream>
#include <whisper.h>
#include "nlohmann/json.hpp"

std::string get_current_time();
long get_current_time_millis();
std::string to_timestamp(int64_t t, bool comma = false);
nlohmann::json get_result(whisper_context *ctx);