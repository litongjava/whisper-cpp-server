```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-tiny.en-q5_1 -f distribute/docker/tiny.en-q5_1/Dockerfile .
```

```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-tiny.en-q5_1
```