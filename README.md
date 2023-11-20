# whisper-cpp service
## simplest
```shell
cmake-build-debug/simplest -m models/ggml-base.en.bin samples/jfk.wav
```
```
simplest [options] file0.wav file1.wav ...

options:                                                                                                                                                                                                
-h,        --help              [default] show this help message and exit                                                                                                                              
-m FNAME,  --model FNAME       [models/ggml-base.en.bin] model path                                                                                                                                   
-di,       --diarize           [false  ] stereo audio diarization
```
## service

Simple http service. WAV Files are passed to the inference model via http requests.

```
./cmake-build-debug/service -m models/ggml-base.en.bin
```

```shell
whisper_init_from_file_with_params_no_state: loading model from 'models/ggml-base.en.bin'
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
whisper_backend_init: using Metal backend
ggml_metal_init: allocating
ggml_metal_init: found device: Apple M2
ggml_metal_init: picking default device: Apple M2
ggml_metal_init: default.metallib not found, loading from source
ggml_metal_init: error: could not use bundle path to find ggml-metal.metal, falling back to trying cwd
ggml_metal_init: loading 'ggml-metal.metal'
ggml_metal_init: GPU name:   Apple M2
ggml_metal_init: GPU family: MTLGPUFamilyApple8 (1008)
ggml_metal_init: hasUnifiedMemory              = true
ggml_metal_init: recommendedMaxWorkingSetSize  = 11453.25 MB
ggml_metal_init: maxTransferRate               = built-in GPU
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =   156.68 MB, (  157.20 / 11453.25)
whisper_model_load:    Metal buffer size =   156.67 MB
whisper_model_load: model size    =  156.58 MB
whisper_backend_init: using Metal backend
ggml_metal_init: allocating
ggml_metal_init: found device: Apple M2
ggml_metal_init: picking default device: Apple M2
ggml_metal_init: default.metallib not found, loading from source
ggml_metal_init: error: could not use bundle path to find ggml-metal.metal, falling back to trying cwd
ggml_metal_init: loading 'ggml-metal.metal'
ggml_metal_init: GPU name:   Apple M2
ggml_metal_init: GPU family: MTLGPUFamilyApple8 (1008)
ggml_metal_init: hasUnifiedMemory              = true
ggml_metal_init: recommendedMaxWorkingSetSize  = 11453.25 MB
ggml_metal_init: maxTransferRate               = built-in GPU
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =    16.52 MB, (  173.72 / 11453.25)
whisper_init_state: kv self size  =   16.52 MB
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =    18.43 MB, (  192.15 / 11453.25)
whisper_init_state: kv cross size =   18.43 MB
whisper_init_state: loading Core ML model from 'models/ggml-base.en-encoder.mlmodelc'
whisper_init_state: first run on a device may take a while ...
whisper_init_state: Core ML model loaded
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =     0.02 MB, (  196.51 / 11453.25)
whisper_init_state: compute buffer (conv)   =    5.67 MB
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =     0.02 MB, (  196.53 / 11453.25)
whisper_init_state: compute buffer (cross)  =    4.71 MB
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =     0.02 MB, (  196.54 / 11453.25)
whisper_init_state: compute buffer (decode) =   96.41 MB
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =     4.05 MB, (  200.59 / 11453.25)
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =     3.08 MB, (  203.67 / 11453.25)
ggml_metal_add_buffer: allocated 'backend         ' buffer, size =    94.78 MB, (  298.45 / 11453.25)

whisper service listening at http://0.0.0.0:8080

Received request: jfk.wav
Successfully loaded jfk.wav

system_info: n_threads = 4 / 8 | AVX = 0 | AVX2 = 0 | AVX512 = 0 | FMA = 0 | NEON = 1 | ARM_FMA = 1 | METAL = 1 | F16C = 0 | FP16_VA = 1 | WASM_SIMD = 0 | BLAS = 1 | SSE3 = 0 | SSSE3 = 0 | VSX = 0 | CUDA = 0 | COREML = 1 | OPENVINO = 0 | 

handleInference: processing 'jfk.wav' (176000 samples, 11.0 sec), 4 threads, 1 processors, lang = en, task = transcribe, timestamps = 1 ...

Running whisper.cpp inference on jfk.wav

[00:00:00.000 --> 00:00:11.000]   And so my fellow Americans, ask not what your country can do for you, ask what you can do for your country.
```
```
./service -h

usage: ./bin/service [options]

options:
  -h,        --help              [default] show this help message and exit
  -t N,      --threads N         [4      ] number of threads to use during computation
  -p N,      --processors N      [1      ] number of processors to use during computation
  -ot N,     --offset-t N        [0      ] time offset in milliseconds
  -on N,     --offset-n N        [0      ] segment index offset
  -d  N,     --duration N        [0      ] duration of audio to process in milliseconds
  -mc N,     --max-context N     [-1     ] maximum number of text context tokens to store
  -ml N,     --max-len N         [0      ] maximum segment length in characters
  -sow,      --split-on-word     [false  ] split on word rather than on token
  -bo N,     --best-of N         [2      ] number of best candidates to keep
  -bs N,     --beam-size N       [-1     ] beam size for beam search
  -wt N,     --word-thold N      [0.01   ] word timestamp probability threshold
  -et N,     --entropy-thold N   [2.40   ] entropy threshold for decoder fail
  -lpt N,    --logprob-thold N   [-1.00  ] log probability threshold for decoder fail
  -debug,    --debug-mode        [false  ] enable debug mode (eg. dump log_mel)
  -tr,       --translate         [false  ] translate from source language to english
  -di,       --diarize           [false  ] stereo audio diarization
  -tdrz,     --tinydiarize       [false  ] enable tinydiarize (requires a tdrz model)
  -nf,       --no-fallback       [false  ] do not use temperature fallback while decoding
  -ps,       --print-special     [false  ] print special tokens
  -pc,       --print-colors      [false  ] print colors
  -pp,       --print-progress    [false  ] print progress
  -nt,       --no-timestamps     [false  ] do not print timestamps
  -l LANG,   --language LANG     [en     ] spoken language ('auto' for auto-detect)
  -dl,       --detect-language   [false  ] exit after automatically detecting language
             --prompt PROMPT     [       ] initial prompt
  -m FNAME,  --model FNAME       [models/ggml-base.en.bin] model path
  -oved D,   --ov-e-device DNAME [CPU    ] the OpenVINO device used for encode inference
  --host HOST,                   [127.0.0.1] Hostname/ip-adress for the service
  --port PORT,                   [8080   ] Port number for the service
```

## request examples

**/inference**
```
curl --location --request POST http://127.0.0.1:8080/inference \
--form file=@"./samples/jfk.wav" \
--form temperature="0.2" \
--form response-format="json"
```

**/load**
```
curl 127.0.0.1:8080/load \
-H "Content-Type: multipart/form-data" \
-F model="<path-to-model-file>"
```