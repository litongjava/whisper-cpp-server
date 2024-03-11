FROM ubuntu:latest

WORKDIR /app

COPY cmake-build-release/ggml-metal.metal /app/
COPY cmake-build-release/simplest /app/
COPY cmake-build-release/whisper_http_server_base_httplib /app/
COPY cmake-build-release/whisper_server_base_on_uwebsockets /app/
COPY cmake-build-release/thirdparty/whisper.cpp/libwhisper.so /lib