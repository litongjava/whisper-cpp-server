# whisper-cpp-server http interface

## POST /inference

POST /inference

> Body 请求参数

```yaml
file: file://1.m4a
temperature: "0.2"
response-format: json
audio_format: m4a

```

### 请求参数

| 名称                | 位置   | 类型             | 必选 | 说明                                   |
|-------------------|------|----------------|----|--------------------------------------|
| body              | body | object         | 否  | none                                 |
| » file            | body | string(binary) | 否  | filename                             |
| » temperature     | body | string         | 否  | none                                 |
| » response-format | body | string         | 否  | none                                 |
| » audio_format    | body | string         | 否  | audio format,support m4a,mp3,and wav |

> 返回示例

> 成功

```json
{
  "code": 0,
  "data": [
    {
      "sentence": " And so my fellow Americans ask not what your country can do for you, ask what you can do for your country.",
      "t0": 0,
      "t1": 1100
    }
  ]
}
```

### 返回结果

| 状态码 | 状态码含义                                                   | 说明 | 数据模型   |
|-----|---------------------------------------------------------|----|--------|
| 200 | [OK](https://tools.ietf.org/html/rfc7231#section-6.3.1) | 成功 | Inline |

