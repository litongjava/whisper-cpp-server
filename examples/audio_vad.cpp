#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <filesystem>

int main(int argc, char **argv) {
//default cmake-build-debug/main
  const char filename[] = "../pcm/16k_1.pcm";
  const char output_dir[] = "output_pcm";
  const char output_filename_prefix[] = "16k_1.pcm";
  if (!std::filesystem::exists(output_dir)) {
    std::filesystem::create_directories(output_dir);
  }
}