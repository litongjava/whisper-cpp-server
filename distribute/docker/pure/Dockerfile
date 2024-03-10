FROM ubuntu:latest

WORKDIR /app

COPY ggml-metal.metal /app/
COPY cmake-build-release/simplest /app/
COPY cmake-build-release/stream_local /app/
COPY cmake-build-release/whisper_http_server_base_httplib /app/
COPY cmake-build-release/audio_vad /app/
COPY cmake-build-release/sdl_version /app/