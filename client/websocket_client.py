# python3 websocket_client.py --server_ip 127.0.0.1 --port 8090 --wavfile ./zh.wav
import argparse
import asyncio
import codecs
import functools
import json
import logging
import os
import time

import numpy as np
import soundfile
import websockets


class Logger(object):
  def __init__(self, name: str = None):
    name = 'PaddleSpeech' if not name else name
    self.logger = logging.getLogger(name)

    log_config = {
      'DEBUG': 10,
      'INFO': 20,
      'TRAIN': 21,
      'EVAL': 22,
      'WARNING': 30,
      'ERROR': 40,
      'CRITICAL': 50,
      'EXCEPTION': 100,
    }
    for key, level in log_config.items():
      logging.addLevelName(level, key)
      if key == 'EXCEPTION':
        self.__dict__[key.lower()] = self.logger.exception
      else:
        self.__dict__[key.lower()] = functools.partial(self.__call__,
                                                       level)

    self.format = logging.Formatter(
      fmt='[%(asctime)-15s] [%(levelname)8s] - %(message)s')

    self.handler = logging.StreamHandler()
    self.handler.setFormatter(self.format)

    self.logger.addHandler(self.handler)
    self.logger.setLevel(logging.INFO)
    self.logger.propagate = False

  def __call__(self, log_level: str, msg: str):
    self.logger.log(log_level, msg)


class ASRWsAudioHandler:
  def __init__(self,
               logger=None,
               url=None,
               port=None,
               endpoint="/paddlespeech/asr/streaming", ):
    """Online ASR Server Client  audio handler
       Online asr server use the websocket protocal
    Args:
        url (str, optional): the server ip. Defaults to None.
        port (int, optional): the server port. Defaults to None.
        endpoint(str, optional): to compatiable with python server and c++ server.
    """
    self.url = url
    self.port = port
    self.logger = logger
    if url is None or port is None or endpoint is None:
      self.url = None
    else:
      self.url = "ws://" + self.url + ":" + str(self.port) + endpoint
    self.logger.info(f"endpoint: {self.url}")

  def read_wave(self, wavfile_path: str):
    """read the audio file from specific wavfile path

    Args:
        wavfile_path (str): the audio wavfile,
                             we assume that audio sample rate matches the model

    Yields:
        numpy.array: the samall package audio pcm data
    """
    samples, sample_rate = soundfile.read(wavfile_path, dtype='int16')
    x_len = len(samples)
    assert sample_rate == 16000

    chunk_size = int(85 * sample_rate / 1000)  # 85ms, sample_rate = 16kHz

    if x_len % chunk_size != 0:
      padding_len_x = chunk_size - x_len % chunk_size
    else:
      padding_len_x = 0

    padding = np.zeros((padding_len_x), dtype=samples.dtype)
    padded_x = np.concatenate([samples, padding], axis=0)

    assert (x_len + padding_len_x) % chunk_size == 0
    num_chunk = (x_len + padding_len_x) / chunk_size
    num_chunk = int(num_chunk)
    for i in range(0, num_chunk):
      start = i * chunk_size
      end = start + chunk_size
      x_chunk = padded_x[start:end]
      yield x_chunk

  async def run(self, wavfile_path: str):
    """Send a audio file to online server

    Args:
        wavfile_path (str): audio path

    Returns:
        str: the final asr result
    """
    logging.debug("send a message to the server")

    results = []
    if self.url is None:
      self.logger.error("No asr server, please input valid ip and port")
      return results

    # 1. send websocket handshake protocal
    start_time = time.time()
    async with websockets.connect(self.url) as ws:
      # 2. server has already received handshake protocal
      # client start to send the command
      audio_info = json.dumps(
        {
          "name": "test.wav",
          "signal": "start",
          "nbest": 1
        },
        sort_keys=True,
        indent=4,
        separators=(',', ': '))
      await ws.send(audio_info)
      msg = await ws.recv()
      self.logger.info("client receive msg={}".format(msg))

      # 3. send chunk audio data to engine
      for chunk_data in self.read_wave(wavfile_path):
        await ws.send(chunk_data.tobytes())
        msg = await ws.recv()
        if msg:
          try:
            json_object = json.loads(msg)
            self.logger.info("client receive msg={}".format(json_object))
            if "result" in json_object:
              result = json_object.get("result")
              for sentence in result:
                # print(type(sentence))
                sentence = "{}->{}:{}".format(sentence['t0'], sentence['t1'], sentence['sentence'])
                results.append(sentence)
                # print(sentence)
          except Exception as e:
            self.logger.error("Unexpected error: {}".format(e))

      # 4. we must send finished signal to the server
      audio_info = json.dumps(
        {
          "name": "test.wav",
          "signal": "end",
          "nbest": 1
        },
        sort_keys=True,
        indent=4,
        separators=(',', ': '))
      await ws.send(audio_info)
      msg = await ws.recv()

      # 5. decode the bytes to str
      json_object = json.loads(msg)

      # 6. logging the final result and comptute the statstics
      elapsed_time = time.time() - start_time
      audio_info = soundfile.info(wavfile_path)
      self.logger.info("client final receive msg={}".format(json_object))

      # print(type(json_object))
      if "result" in json_object:
        result = json_object.get("result")
        for sentence in result:
          # print(type(sentence))
          sentence = "{}->{}:{}".format(sentence['t0'], sentence['t1'], sentence['sentence'])
          results.append(sentence)
          # print(sentence)
      self.logger.info(
        f"audio duration: {audio_info.duration}, elapsed time: {elapsed_time}, RTF={elapsed_time / audio_info.duration}"
      )
      return results


logger = Logger()


def main(args):
  logger.info("asr websocket client start")
  handler = ASRWsAudioHandler(
    logger,
    args.server_ip,
    args.port,
    endpoint=args.endpoint)
  loop = asyncio.get_event_loop()

  # support to process single audio file
  if args.wavfile and os.path.exists(args.wavfile):
    logger.info(f"start to process the wavscp: {args.wavfile}")
    results = loop.run_until_complete(handler.run(args.wavfile))
    if results:
      for sentence in results:
        print(sentence)

  # support to process batch audios from wav.scp
  if args.wavscp and os.path.exists(args.wavscp):
    logger.info(f"start to process the wavscp: {args.wavscp}")
    with codecs.open(args.wavscp, 'r', encoding='utf-8') as f, \
      codecs.open("result.txt", 'w', encoding='utf-8') as w:
      for line in f:
        utt_name, utt_path = line.strip().split()
        result = loop.run_until_complete(handler.run(utt_path))
        result = result["result"]
        w.write(f"{utt_name} {result}\n")


if __name__ == "__main__":
  logger.info("Start to do streaming asr client")
  parser = argparse.ArgumentParser()
  parser.add_argument(
    '--server_ip', type=str, default='127.0.0.1', help='server ip')
  parser.add_argument('--port', type=int, default=8090, help='server port')
  parser.add_argument(
    "--endpoint",
    type=str,
    default="/paddlespeech/asr/streaming",
    help="ASR websocket endpoint")
  parser.add_argument(
    "--wavfile",
    action="store",
    help="wav file path ",
    default="./16_audio.wav")
  parser.add_argument(
    "--wavscp", type=str, default=None, help="The batch audios dict text")
  args = parser.parse_args()

  main(args)
