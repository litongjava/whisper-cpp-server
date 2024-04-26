```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-medium-q5_0 -f distribute/docker/medium-q5_0/Dockerfile .
```

```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-medium-q5_0
```