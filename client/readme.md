
```shell
pip install soundfile
pip install websockets
```
```shell script
python client\websocket_client.py --server_ip 127.0.0.1 --port 8090 --wavfile samples/jfk.wav
```
only save audio
```shell script
python client\websocket_client.py --server_ip 127.0.0.1 --port 8090 --endpoint /streaming/save --wavfile samples/jfk.wav
```