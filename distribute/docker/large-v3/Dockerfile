FROM litongjava/whisper-cpp-server-builder:1.0.0 as builder

COPY . /src

WORKDIR /src
RUN cmake -B cmake-build-release-linux
RUN cmake --build cmake-build-release-linux --config Release -- -j $(nproc)

FROM ubuntu:latest as runtime
WORKDIR /app

COPY models/ggml-large-v3.bin /app/models/
COPY ggml-metal.metal /app/

COPY --from=builder /src/cmake-build-release-linux/simplest /app/
COPY --from=builder /src/cmake-build-release-linux/stream_local /app/
COPY --from=builder /src/cmake-build-release-linux/whisper_http_server_base_httplib /app/
COPY --from=builder /src/cmake-build-release-linux/audio_vad /app/
COPY --from=builder /src/cmake-build-release-linux/sdl_version /app/

EXPOSE 8080

CMD ["/app/whisper_http_server_base_httplib", "-m", "/app/models/ggml-base.en.bin"]
