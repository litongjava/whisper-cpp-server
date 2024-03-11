```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-base-en -f distribute/docker/base.en/Dockerfile .
```

```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-base-en
```