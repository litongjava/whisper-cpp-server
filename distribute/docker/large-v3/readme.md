```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-large-v3 -f distribute/docker/large-v3/Dockerfile .
```

## test run
```
docker run -dit --net=host --name=whisper-cpp-server litongjava/whisper-cpp-server:1.0.0-large-v3
```

