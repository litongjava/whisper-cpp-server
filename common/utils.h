#pragma once

#include <iostream>
#include <whisper.h>
#include "../nlohmann/json.hpp"

std::string get_current_time();
long get_current_time_millis();
nlohmann::json get_result(whisper_context *ctx);