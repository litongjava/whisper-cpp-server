# paddlespeech serving 网页Demo

![图片](./paddle_web_demo.png)

step1: 开启流式语音识别服务器端

```
# 开启流式语音识别服务
./cmake-build-debug/whisper_server_base_on_uwebsockets -m models/ggml-base.en.bin
```

step2: 谷歌游览器打开 `web`目录下`index.html`

step3: 点击`连接`，验证WebSocket是否成功连接

step4：点击开始录音(弹窗询问，允许录音)


 
