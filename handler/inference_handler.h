#pragma once

#include "httplib.h"
#include "../params/whisper_params.h"

using namespace httplib;

void handleInference(const Request &request, Response &response, std::mutex &whisper_mutex, whisper_params &params,
                     whisper_context *ctx, char *arg_audio_file);
