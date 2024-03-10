FROM ubuntu:latest

WORKDIR /app

COPY ggml-metal.metal /app/
COPY cmake-build-release/simplest /app/
COPY cmake-build-release/whisper_http_server_base_httplib /app/
COPY cmake-build-release/whisper_server_base_on_uwebsockets /app/
copy cmake-build-release/libwhisper.so /app
