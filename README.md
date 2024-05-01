# whisper-cpp service
## open sourde address
[github](https://github.com/ppnt/whisper-cpp-server)
[gitee](https://gitee.com/ppnt/whisper-cpp-server)
## Whisper-CPP-Server Introduction
Whisper-CPP-Server is a high-performance speech recognition service written in C++, designed to provide developers and enterprises with a reliable and efficient speech-to-text inference engine. This project implements technology from ggml to perform inference on the open-source Whisper model. While ensuring speed and accuracy, it supports pure CPU-based inference operations, allowing for high-quality speech recognition services without the need for specialized hardware accelerators.

Real-time speech recognition and display of recognition results in the browser backend
```
https://github.com/litongjava/whisper-cpp-server
```
frontend
```
https://github.com/litongjava/listen-know-web
```
Test video  

https://github.com/litongjava/whisper-cpp-server/assets/31761981/ba7268fa-312c-47b2-a538-804b96bb656f


## Main Features
1.Pure C++ Inference Engine
Whisper-CPP-Server is entirely written in C++, leveraging the efficiency of C++ for rapid processing of vast amounts of voice data, even in environments that only have CPUs for computing power.

2.High Performance
Thanks to the computational efficiency of C++, Whisper-CPP-Server can offer exceptionally high processing speeds, meeting real-time or near-real-time speech recognition demands. It is especially suited for scenarios that require processing large volumes of voice data.

3.Support for Multiple Languages
The service supports speech recognition in multiple languages, broadening its applicability across various linguistic contexts.

4.Docker Container Support
A Docker image is provided, enabling quick deployment of the service through simple command-line operations, significantly simplifying installation and configuration processes. Deploy using the following command:
```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-large-v3
```
This means you can run Whisper-CPP-Server on any platform that supports Docker, including but not limited to Linux, Windows, and macOS.

4.Easy Integration for Clients
Detailed client integration documentation is provided, helping developers quickly incorporate speech recognition functionality into their applications.
[Client Code Documentation](https://github.com/litongjava/whisper-cpp-server/blob/main/doc/client_code.md)

## Applicable Scenarios
Whisper-CPP-Server is suitable for a variety of applications that require fast and accurate speech recognition, including but not limited to:

- Voice-driven interactive applications
- Transcription of meeting records
- Automatic subtitle generation
- Automatic translation of multi-language content

## How to build it 
build with cmake and vcpkg
```
git clone https://github.com/litongjava/whisper-cpp-server.git
git submodule init
git submodule update
cmake -B cmake-build-release
cp ./ggml-metal.metal cmake-build-release 
cmake --build cmake-build-release --config Release -- -j 12
```

run with simplest
```
./cmake-build-release/simplest -m models/ggml-base.en.bin test.wav
```

run with http-server
```
./cmake-build-release/whisper_http_server_base_httplib -m models/ggml-base.en.bin 
```

run with websocket-server
```
./cmake-build-release/whisper_server_base_on_uwebsockets -m models/ggml-base.en.bin
```

copy command
```
mkdir bin
cp ./ggml-metal.metal bin
cp ./cmake-build-release/simplest bin
cp ./cmake-build-release/whisper_http_server_base_httplib bin 
cp ./cmake-build-release/whisper_server_base_on_uwebsockets bin
```

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
## whisper_http_server_base_httplib

Simple http service. WAV mp4 and m4a Files are passed to the inference model via http requests.

```
./whisper_http_server_base_httplib -h

usage: ./bin/whisper_http_server_base_httplib [options]

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
## start whisper_http_server_base_httplib
```
./cmake-build-debug/whisper_http_server_base_httplib -m models/ggml-base.en.bin
```
Test server  
see request doc in [doc](doc)
## request examples

**/inference**
```
curl --location --request POST http://127.0.0.1:8080/inference \
--form file=@"./samples/jfk.wav" \
--form temperature="0.2" \
--form response-format="json"
--form audio_format="wav"
```

**/load**
```
curl 127.0.0.1:8080/load \
-H "Content-Type: multipart/form-data" \
-F model="<path-to-model-file>"
```

## whisper_server_base_on_uwebsockets
web socket server  
start server
```
./cmake-build-debug/whisper_server_base_on_uwebsockets -m models/ggml-base.en.bin
```
Test server
see python [client](client)

## Docker
### run whisper-cpp-server:1.0.0
[Dockerfile](./distribute/docker/pure/)
```
docker run -dit --name=whisper-server -p 8080:8080 -v "$(pwd)/models/ggml-base.en.bin":/models/ggml-base.en.bin litongjava/whisper-cpp-server:1.0.0 /app/whisper_http_server_base_httplib -m /models/ggml-base.en.bin
```
the port is 8080
### test
```
curl --location --request POST 'http://127.0.0.1:8080/inference' \
--header 'Accept: */*' \
--header 'Content-Type: multipart/form-data; boundary=--------------------------671827497522367123871197' \
--form 'file=@"E:\\code\\cpp\\cpp-study\\cpp-study-clion\\audio\\jfk.wav"' \
--form 'temperature="0.2"' \
--form 'response-format="json"' \
--form 'audio_format="wav"'
```
### run whisper-cpp-server:1.0.0-base-en
[Dockerfile](./distribute/docker/base.en/)
```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-base-en
```

### run whisper-cpp-server:1.0.0-large-v3
[Dockerfile](./distribute/docker/large-v3/)
```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-large-v3
```
## run whisper-cpp-server:1.0.0-tiny.en-q5_1
[Dockerfile](./distribute/docker/tiny.en-q5_1/)
```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-tiny.en-q5_1
```
### Client code
[Client code](./doc/client_code.md)
