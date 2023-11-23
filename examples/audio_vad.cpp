#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <filesystem>

extern "C" {
#include "simplevad/simple_vad.h"
#include "simplevad/period_format.h"
#include "simplevad/file_cut.h"
}
int run(FILE *fp, simple_vad *vad, struct cut_info *cut);

int add_period_activity(struct periods *per, int is_active, int is_last);

int main(int argc,char** argv) {
//default cmake-build-debug/main
  const char filename[] = "../pcm/16k_1.pcm";
  const char output_dir[] = "output_pcm";
  const char output_filename_prefix[] = "16k_1.pcm";
  if (!std::filesystem::exists(output_dir)) {
    std::filesystem::create_directories(output_dir);
  }

  std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename, "rb"), &fclose);
  if (!fp) {
    std::cerr << filename << " does not exist\n";
    return 3;
  }

  std::unique_ptr<simple_vad, decltype(&simple_vad_free)> vad(
    simple_vad_create(), &simple_vad_free);
  if (!vad) {
    return 4;
  }

  std::unique_ptr<FILE, decltype(&fclose)> fp2(fopen(filename, "rb"), &fclose);
  std::unique_ptr<struct cut_info, decltype(&cut_info_free)> cut(cut_info_create(fp2.get()), &cut_info_free);

  snprintf(cut->output_filename_prefix, sizeof(cut->output_filename_prefix), "%s",
           output_filename_prefix);
  snprintf(cut->output_file_dir, sizeof(cut->output_file_dir), "%s", output_dir);

  int res = run(fp.get(), vad.get(), cut.get());

  std::cout << "PROGRAM FINISH\n";
  return res;
}

int run(FILE *fp, simple_vad *vad, struct cut_info *cut) {
  int16_t data[FRAME_SIZE];
  int res = 0;
  struct periods *per = periods_create();

  while (res == 0) {
    res = read_int16_bytes(fp, data);
    if (res <= 1) {
      int is_last = (res == 1);
      int is_active = process_vad(vad, data);
      add_period_activity(per, is_active, is_last);
      int vad_file_res = cut_add_vad_activity(cut, is_active, is_last);
      if (vad_file_res < 0) {
        std::cout << "file write success " << cut->result_filename << "\n";
      }
    } else if (ferror(fp)) {
      std::cout << "read failed  ferror result  : " << ferror(fp) << "\n";
    }
  }

  periods_free(per);

  if (res != 1) {
    std::cerr << "read file error " << res << "\n";
    return res;
  }
  return 0;
}

int add_period_activity(struct periods *per, int is_active, int is_last) {
  static int old_is_active = 0;
  static int count = 0;
  int res_add = period_add_vad_activity(per, is_active, is_last);
  if (res_add != 0) {
    return res_add;
  }
  if (is_active != old_is_active) {
    old_is_active = is_active;
  }
  count += 1;
  if (is_last) {
    //periods_print(per);
    std::cout << "total frames " << count << "\n";
  }
}