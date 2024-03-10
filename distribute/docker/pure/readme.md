```shell
docker build -t litongjava/whisper-cpp-server:1.0.0 -f distribute/docker/pure/Dockerfile .
```

test
```
docker run --rm -v models:/models -v jfk.wav:/jfk.wav litongjava/whisper-cpp-server:1.0.0 simplest -m /models/ggml-base.en.bin /jfk.wav
```