# whisper-cpp-server HTTP Interface

## POST /inference

POST /inference

> Body Request Parameters

```yaml
file: file://1.m4a
temperature: "0.2"
response-format: json
audio_format: m4a
```

### Request Parameters

| Name              | Location | Type           | Required | Description                             |
|-------------------|----------|----------------|----------|-----------------------------------------|
| body              | body     | object         | No       | none                                    |
| » file            | body     | string(binary) | No       | filename                                |
| » temperature     | body     | string         | No       | none                                    |
| » response-format | body     | string         | No       | none                                    |
| » audio_format    | body     | string         | No       | audio format, support m4a, mp3, and wav |

> Response Example

> Success

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

### Response Result

| Status Code | Status Code Meaning                                     | Description | Data Model |
|-------------|---------------------------------------------------------|-------------|------------|
| 200         | [OK](https://tools.ietf.org/html/rfc7231#section-6.3.1) | Success     | Inline     |