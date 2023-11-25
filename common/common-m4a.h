#ifndef WHISPER_CPP_SERVER_COMMON_M4A_H
#define WHISPER_CPP_SERVER_COMMON_M4A_H
#include <vector>
#include <string>
bool read_m4a(const std::string &fname, std::vector<float> &pcmf32, std::vector<std::vector<float>> &pcmf32s,
              bool stereo);
#endif //WHISPER_CPP_SERVER_COMMON_M4A_H
