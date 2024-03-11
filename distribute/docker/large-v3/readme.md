```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-large-v3 -f distribute/docker/large-v3/Dockerfile .
```

## test run
```
docker run -dit --name whisper-server -p 8080:8080 litongjava/whisper-cpp-server:1.0.0-large-v3
```



