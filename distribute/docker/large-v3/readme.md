```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-large-v3 -f distribute/docker/large-v3/Dockerfile .
```

## test build

docker run -dit --net=host -v .:/src litongjava/whisper-cpp-server-builder:1.0.0 /bin/sh