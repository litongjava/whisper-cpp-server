# Streaming Voice Recognition Interface

## 1.1. Create Connection

- ws api: `ws://{server}:{port}/paddlespeech/asr/streaming`

## 1.2. Start Signal

The client sends streaming voice recognition audio information and decoding parameters through the start signal.

### Request Example

```json
{
  "name": "test.wav",
  "signal": "start",
  "nbest": 1
}
```

### Server Information

The server returns the status of the new connection.

#### Field Description

| Field  | Required | Type   | Description                              |
| ------ | -------- | ------ | ---------------------------------------- |
| status | Yes      | string | ASR server status                        |
| signal | Yes      | string | The streaming connection preparation is complete |

```json
{
  "status": "ok", 
  "signal": "server_ready"
}
```

The server also needs to save the WAV file.

## 1.3. Data

After the client and server establish a connection, the client continuously sends data to the server.

### Client Information

Send PCM16 data stream to the server.

### Server Information

For each data sent, the server returns the decoding result of that data packet.

#### Field Description

| Field  | Required | Type   | Description      |
| ------ | -------- | ------ | ---------------- |
| result | Yes      | string | ASR decoding result |

## 1.4. End

After sending the last data packet, the client needs to send an end command to the server to notify the server to destroy the resources associated with that connection.

Inputting Streaming Voice Recognition Audio Information and Decoding Parameters Through the Start Signal

#### Field Description

| Field  | Required | Type   | Description                          |
| ------ | -------- | ------ | ------------------------------------ |
| name   | Yes      | string | Name of the input audio              |
| signal | Yes      | string | Type of command in streaming recognition |
| nbest  | Yes      | int    | Recognition nbest parameter, default is 1 |

```json
{
  "name": "test.wav",
  "signal": "end",
  "nbest": 1
}
```

### Server Information

After receiving the end information, the server sends back the final result.

#### Field Description

| Field  | Required | Type   | Description                                   |
| ------ | -------- | ------ | --------------------------------------------- |
| name   | Yes      | string | Name of the input audio                       |
| signal | Yes      | string | Type of command in streaming recognition, as sent |
| result | Yes      | string | The final recognition result                  |

```json
{
  "name": "test.wav",
  "signal": "end",
  "result": ""
}
```