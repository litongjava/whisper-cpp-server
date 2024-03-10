FROM litongjava/whisper-cpp-server-builder:1.0.0 as builder

WORKDIR /src
RUN cmake -B cmake-build-release-linux
RUN cmake --build cmake-build-release-linux --config Release -- -j $(nproc)
