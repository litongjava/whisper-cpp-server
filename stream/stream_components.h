#pragma once

namespace stream_components {
  struct whisper_local_stream_params {
    audio_params audio;
    service_params service;

    void initialize() {
      audio.initialize();
      service.initialize();
    }
  };

  void whisper_print_usage(int /*argc*/, char **argv, const whisper_local_stream_params &params) {
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [options]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "  -h,       --help          [default] show this help message and exit\n");
    fprintf(stderr, "  -t N,     --threads N     [%-7d] number of threads to use during computation\n",
            params.service.n_threads);
    fprintf(stderr, "            --step N        [%-7d] audio step size in milliseconds\n", params.audio.step_ms);
    fprintf(stderr, "            --length N      [%-7d] audio length in milliseconds\n", params.audio.length_ms);
    fprintf(stderr, "            --keep N        [%-7d] audio to keep from previous step in ms\n",
            params.audio.keep_ms);
    fprintf(stderr, "  -c ID,    --capture ID    [%-7d] capture device ID\n", params.audio.capture_id);
    //fprintf(stderr, "  -mt N,    --max-tokens N  [%-7d] maximum number of tokens per audio chunk\n",       params.max_tokens);
    fprintf(stderr, "  -ac N,    --audio-ctx N   [%-7d] audio context size (0 - all)\n", params.audio.audio_ctx);
    fprintf(stderr, "  -vth N,   --vad-thold N   [%-7.2f] voice activity detection threshold\n",
            params.audio.vad_thold);
    fprintf(stderr, "  -fth N,   --freq-thold N  [%-7.2f] high-pass frequency cutoff\n", params.audio.freq_thold);
    fprintf(stderr, "  -su,      --speed-up      [%-7s] speed up audio by x2 (reduced accuracy)\n",
            params.service.speed_up ? "true" : "false");
    fprintf(stderr, "  -tr,      --translate     [%-7s] translate from source language to english\n",
            params.service.translate ? "true" : "false");
    fprintf(stderr, "  -nf,      --no-fallback   [%-7s] do not use temperature fallback while decoding\n",
            params.service.no_fallback ? "true" : "false");
    //fprintf(stderr, "  -ps,      --print-special [%-7s] print special tokens\n",                           params.print_special ? "true" : "false");
    fprintf(stderr, "  -kc,      --keep-context  [%-7s] keep context between audio chunks\n",
            params.service.no_context ? "false" : "true");
    fprintf(stderr, "  -l LANG,  --language LANG [%-7s] spoken language\n", params.service.language.c_str());
    fprintf(stderr, "  -m FNAME, --model FNAME   [%-7s] model path\n", params.service.model.c_str());
    //fprintf(stderr, "  -f FNAME, --file FNAME    [%-7s] text output file name\n",                          params.fname_out.c_str());
    fprintf(stderr, "  -tdrz,     --tinydiarize  [%-7s] enable tinydiarize (requires a tdrz model)\n",
            params.service.tinydiarize ? "true" : "false");
    //fprintf(stderr, "  -sa,      --save-audio    [%-7s] save the recorded audio to a file\n",              params.save_audio ? "true" : "false");
    fprintf(stderr, "\n");
  }

  bool whisper_params_parse(int argc, char **argv, whisper_local_stream_params &params) {
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];

      if (arg == "-h" || arg == "--help") {
        whisper_print_usage(argc, argv, params);
        exit(0);
      } else if (arg == "-t" || arg == "--threads") { params.service.n_threads = std::stoi(argv[++i]); }
      else if (arg == "-p" || arg == "--processors") { params.service.n_processors = std::stoi(argv[++i]); }
      else if (arg == "--step") { params.audio.step_ms = std::stoi(argv[++i]); }
      else if (arg == "--length") { params.audio.length_ms = std::stoi(argv[++i]); }
      else if (arg == "--keep") { params.audio.keep_ms = std::stoi(argv[++i]); }
      else if (arg == "-c" || arg == "--capture") { params.audio.capture_id = std::stoi(argv[++i]); }
        //else if (arg == "-mt"  || arg == "--max-tokens")    { params.max_tokens    = std::stoi(argv[++i]); }
      else if (arg == "-ac" || arg == "--audio-ctx") { params.audio.audio_ctx = std::stoi(argv[++i]); }
      else if (arg == "-vth" || arg == "--vad-thold") { params.audio.vad_thold = std::stof(argv[++i]); }
      else if (arg == "-fth" || arg == "--freq-thold") { params.audio.freq_thold = std::stof(argv[++i]); }
      else if (arg == "-su" || arg == "--speed-up") { params.service.speed_up = true; }
      else if (arg == "-tr" || arg == "--translate") { params.service.translate = true; }
      else if (arg == "-nf" || arg == "--no-fallback") { params.service.no_fallback = true; }
        //else if (arg == "-ps"  || arg == "--print-special") { params.print_special = true; }
      else if (arg == "-kc" || arg == "--keep-context") { params.service.no_context = false; }
      else if (arg == "-l" || arg == "--language") { params.service.language = argv[++i]; }
      else if (arg == "-m" || arg == "--model") { params.service.model = argv[++i]; }
        //else if (arg == "-f"   || arg == "--file")          { params.fname_out     = argv[++i]; }
      else if (arg == "-tdrz" || arg == "--tinydiarize") { params.service.tinydiarize = true; }
        //else if (arg == "-sa"  || arg == "--save-audio")    { params.save_audio    = true; }

      else {
        fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
        whisper_print_usage(argc, argv, params);
        exit(0);
      }
    }

    return true;
  }
}