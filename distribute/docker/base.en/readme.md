```shell
docker build -t litongjava/whisper-cpp-server:1.0.0-base-en -f distribute/docker/base.en/Dockerfile .
```

```
docker run --net=host --rm litongjava/whisper-cpp-server:1.0.0-base-en
```