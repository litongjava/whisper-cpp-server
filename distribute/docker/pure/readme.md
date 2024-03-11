## build
```shell
docker build -t litongjava/whisper-cpp-server:1.0.0 -f distribute/docker/pure/Dockerfile .
```
## test
### test with ggml-base.en.bin
```
docker run --rm \
  -v "$(pwd)/models":/models \
  -v "$(pwd)/samples/jfk.wav":/jfk.wav \
  litongjava/whisper-cpp-server:1.0.0 /app/simplest -m /models/ggml-base.en.bin /jfk.wav
```

```
docker run --rm -v "$(pwd)/models/ggml-base.en.bin":/models/ggml-base.en.bin -v "$(pwd)/samples/zh.wav":/samples/zh.wav litongjava/whisper-cpp-server:1.0.0 /app/simplest -m /models/ggml-base.en.bin /samples/zh.wav
```

### test with ggml-large-v3.bin
```
docker run --rm -v "$(pwd)/models/ggml-large-v3.bin":/models/ggml-large-v3.bin -v "$(pwd)/samples/zh.wav":/samples/zh.wav litongjava/whisper-cpp-server:1.0.0 /app/simplest -m /models/ggml-large-v3.bin /samples/zh.wav
```

### test with server
```
docker run -dit --name=whisper-server --net=host -v "$(pwd)/models/ggml-base.en.bin":/models/ggml-base.en.bin litongjava/whisper-cpp-server:1.0.0 /app/whisper_http_server_base_httplib -m /models/ggml-base.en.bin
```

### test outout
#### English
```
root@ping-Inspiron-3458:~/code/whisper-cpp-server# docker run --rm \
>   -v "$(pwd)/models":/models \
>   -v "$(pwd)/jfk.wav":/jfk.wav \
>   litongjava/whisper-cpp-server:1.0.0 /app/simplest -m /models/ggml-base.en.bin /jfk.wav
whisper_init_from_file_with_params_no_state: loading model from '/models/ggml-base.en.bin'
whisper_model_load: loading model
whisper_model_load: n_vocab       = 51864
whisper_model_load: n_audio_ctx   = 1500
whisper_model_load: n_audio_state = 512
whisper_model_load: n_audio_head  = 8
whisper_model_load: n_audio_layer = 6
whisper_model_load: n_text_ctx    = 448
whisper_model_load: n_text_state  = 512
whisper_model_load: n_text_head   = 8
whisper_model_load: n_text_layer  = 6
whisper_model_load: n_mels        = 80
whisper_model_load: ftype         = 1
whisper_model_load: qntvr         = 0
whisper_model_load: type          = 2 (base)
whisper_model_load: adding 1607 extra tokens
whisper_model_load: n_langs       = 99
whisper_model_load:      CPU total size =   147.37 MB

whisper_model_load: model size    =  147.37 MB
whisper_init_state: kv self size  =   16.52 MB
whisper_init_state: kv cross size =   18.43 MB
whisper_init_state: compute buffer (conv)   =   16.39 MB
whisper_init_state: compute buffer (encode) =  132.07 MB
whisper_init_state: compute buffer (cross)  =    4.78 MB
whisper_init_state: compute buffer (decode) =   96.48 MB

system_info: n_threads = 4 / 4 | AVX = 1 | AVX2 = 1 | AVX512 = 0 | FMA = 1 | NEON = 0 | ARM_FMA = 0 | METAL = 0 | F16C = 1 | FP16_VA = 0 | WASM_SIMD = 0 | BLAS = 0 | SSE3 = 1 | SSSE3 = 1 | VSX = 0 | CUDA = 0 | COREML = 0 | OPENVINO = 0 | 

main: WARNING: model is not multilingual, ignoring language and translation options
main: processing '/jfk.wav' (176000 samples, 11.0 sec), 4 threads, 1 processors, lang = en, task = transcribe, timestamps = 1 ...


whisper_print_timings:     load time =   169.51 ms
whisper_print_timings:     fallbacks =   0 p /   0 h
whisper_print_timings:      mel time =    59.75 ms
whisper_print_timings:   sample time =    25.05 ms /     1 runs (   25.05 ms per run)
whisper_print_timings:   encode time =  6384.86 ms /     1 runs ( 6384.86 ms per run)
whisper_print_timings:   decode time =   236.91 ms /    27 runs (    8.77 ms per run)
whisper_print_timings:   batchd time =     0.00 ms /     1 runs (    0.00 ms per run)
whisper_print_timings:   prompt time =     0.00 ms /     1 runs (    0.00 ms per run)
whisper_print_timings:    total time =  6885.22 ms
start

[00:00:00.000 --> 00:00:11.000]   And so my fellow Americans, ask not what your country can do for you, ask what you can do for your country.
```
#### Chinese
```
root@ping-Inspiron-3458:~/code/whisper-cpp-server# docker run --rm -v "$(pwd)/models/ggml-large-v3.bin":/models/ggml-large-v3.bin -v "$(pwd)/samples/zh.wav":/samples/zh.wav litongjava/whisper-cpp-server:1.0.0 /app/simplest -m /models/ggml-large-v3.bin /samples/zh.wav
whisper_init_from_file_with_params_no_state: loading model from '/models/ggml-large-v3.bin'
whisper_model_load: loading model
whisper_model_load: n_vocab       = 51866
whisper_model_load: n_audio_ctx   = 1500
whisper_model_load: n_audio_state = 1280
whisper_model_load: n_audio_head  = 20
whisper_model_load: n_audio_layer = 32
whisper_model_load: n_text_ctx    = 448
whisper_model_load: n_text_state  = 1280
whisper_model_load: n_text_head   = 20
whisper_model_load: n_text_layer  = 32
whisper_model_load: n_mels        = 128
whisper_model_load: ftype         = 1
whisper_model_load: qntvr         = 0
whisper_model_load: type          = 5 (large v3)
whisper_model_load: adding 1609 extra tokens
whisper_model_load: n_langs       = 100
whisper_model_load:      CPU total size =  3094.36 MB
whisper_model_load: model size    = 3094.36 MB
whisper_init_state: kv self size  =  220.20 MB
whisper_init_state: kv cross size =  245.76 MB
whisper_init_state: compute buffer (conv)   =   36.26 MB
whisper_init_state: compute buffer (encode) =  926.66 MB
whisper_init_state: compute buffer (cross)  =    9.38 MB
whisper_init_state: compute buffer (decode) =  209.26 MB

system_info: n_threads = 4 / 4 | AVX = 1 | AVX2 = 1 | AVX512 = 0 | FMA = 1 | NEON = 0 | ARM_FMA = 0 | METAL = 0 | F16C = 1 | FP16_VA = 0 | WASM_SIMD = 0 | BLAS = 0 | SSE3 = 1 | SSSE3 = 1 | VSX = 0 | CUDA = 0 | COREML = 0 | OPENVINO = 0 | 

main: processing '/samples/zh.wav' (79949 samples, 5.0 sec), 4 threads, 1 processors, lang = auto, task = transcribe, timestamps = 1 ...

whisper_full_with_state: auto-detected language: zh (p = 0.998135)
start

[00:00:00.000 --> 00:00:05.000]  我认为跑步最重要的就是给我带来了身体健康

whisper_print_timings:     load time =  5730.34 ms
whisper_print_timings:     fallbacks =   0 p /   0 h
whisper_print_timings:      mel time =    27.16 ms
whisper_print_timings:   sample time =    27.23 ms /     1 runs (   27.23 ms per run)
whisper_print_timings:   encode time = 253393.73 ms /     2 runs (126696.87 ms per run)
whisper_print_timings:   decode time = 11884.19 ms /    21 runs (  565.91 ms per run)
whisper_print_timings:   batchd time =   260.31 ms /     3 runs (   86.77 ms per run)
whisper_print_timings:   prompt time =     0.00 ms /     1 runs (    0.00 ms per run)
whisper_print_timings:    total time = 271354.41 ms

```