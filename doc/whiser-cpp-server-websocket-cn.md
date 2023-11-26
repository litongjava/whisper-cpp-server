# 流式语音识别接口

## 1.1. 创建连接

- ws api： `ws://{server}:{port}/paddlespeech/asr/streaming`

## 1.2. 开始信号

Client 通过开始信号传入流式识别音频信息，以及解码参数。

#### 字段说明

| 字段            | 必选 | 类型     | 说明                                |
|---------------|----|--------|-----------------------------------|
| name          | 是  | string | 传入的音频名称                           |
| signal        | 是  | string | 流式识别中命令类型                         |
| nbest         | 是  | int    | 识别nbest参数，默认是1                    |
| sampleRate    | 否  | int    | 例如48000,默认16000                   |
| bitsPerSample | 否  | int    | 位深度（bitsPerSample）：表示每个样本的位数,默认16 |
| channels      | 否  | int    | 通道数,默认1                           |

### 请求示例

```json
{
  "name": "test.wav",
  "signal": "start",
  "nbest": 1
}
```

### Server 信息

Server 端返回新连接的情况。

#### 字段说明

| 字段     | 必选 | 类型     | 说明                |
|--------|----|--------|-------------------|
| status | 是  | string | ASR服务端状态          |
| signal | 是  | string | 该流式连接必要的准备工作是完成状态 |

```json
{
  "status": "ok",
  "signal": "server_ready"
}
```

服务端同时需要保存wav端的文件。

## 1.3. 数据

Client和Server建立连接之后，Client端不断地向服务端发送数据。

### Client 信息

发送 PCM16 数据流到服务端。

### Server 信息

每发送一个数据，服务端会将该数据包解码的结果返回出来。

#### 字段说明

| 字段     | 必选 | 类型     | 说明       |
|--------|----|--------|----------|
| result | 是  | string | ASR解码的结果 |

## 1.4. 结束

Client 发送完最后一个数据包之后，需要发送给服务端一个结束的命令，通知服务端销毁该链接的相关资源。

通过开始信号传入流式识别音频信息，以及解码参数

#### 字段说明

| 字段     | 必选 | 类型     | 说明             |
|--------|----|--------|----------------|
| name   | 是  | string | 传入的音频名称        |
| signal | 是  | string | 流式识别中命令类型      |
| nbest  | 是  | int    | 识别nbest参数，默认是1 |

```json
{
  "name": "test.wav",
  "signal": "end",
  "nbest": 1
}
```

### Server 信息

Server 端信息接收到结束信息之后，将最后的结果返回出去。

#### 字段说明

| 字段     | 必选 | 类型     | 说明                 |
|--------|----|--------|--------------------|
| name   | 是  | string | 传入的音频名称            |
| signal | 是  | string | 流式识别中命令类型,取发送的命令类型 |
| result | 是  | string | 最后的识别结果            |

```json
{
  "name": "test.wav",
  "signal": "end",
  "result": ""
}
```