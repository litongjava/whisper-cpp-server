#include "common.h"
#include "common-m4a.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#include <vector>
#include <string>

bool read_m4a(const std::string &fname, std::vector<float> &pcmf32, std::vector<std::vector<float>> &pcmf32s,
              bool stereo) {
  avformat_network_init();

  AVFormatContext *formatContext = avformat_alloc_context();
  if (avformat_open_input(&formatContext, fname.c_str(), nullptr, nullptr) != 0) {
    fprintf(stderr, "Could not open file %s\n", fname.c_str());
    return false;
  }

  if (avformat_find_stream_info(formatContext, nullptr) < 0) {
    fprintf(stderr, "Could not find stream information\n");
    avformat_close_input(&formatContext);
    return false;
  }

  const AVCodec *codec = nullptr;
  int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
  if (streamIndex < 0) {
    fprintf(stderr, "Could not find any audio stream in the file\n");
    avformat_close_input(&formatContext);
    return false;
  }

  AVCodecContext *codecContext = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(codecContext, formatContext->streams[streamIndex]->codecpar);

  if (avcodec_open2(codecContext, codec, nullptr) < 0) {
    fprintf(stderr, "Could not open codec\n");
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return false;
  }

  //bool need_resample = (codecContext->sample_rate != COMMON_SAMPLE_RATE);
  SwrContext *swrCtx = nullptr;
  swrCtx = swr_alloc_set_opts(nullptr,
                              stereo ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO,
                              AV_SAMPLE_FMT_FLT,
                              COMMON_SAMPLE_RATE,
                              codecContext->channel_layout,
                              codecContext->sample_fmt,
                              codecContext->sample_rate,
                              0, nullptr);
  if (!swrCtx || swr_init(swrCtx) < 0) {
    fprintf(stderr, "Could not initialize the resampling context\n");
    swr_free(&swrCtx);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    return false;
  }


  AVPacket packet;
  av_init_packet(&packet);
  packet.data = nullptr;
  packet.size = 0;

  AVFrame *frame = av_frame_alloc();

  while (av_read_frame(formatContext, &packet) >= 0) {
    if (packet.stream_index == streamIndex) {
      //decode
      int ret = avcodec_send_packet(codecContext, &packet);
      if (ret < 0) {
        fprintf(stderr, "Error sending packet for decoding\n");
        break;
      }

      while (ret >= 0) {
        ret = avcodec_receive_frame(codecContext, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          break;
        } else if (ret < 0) {
          fprintf(stderr, "Error during decoding\n");
          break;
        }

        // Direct processing of decoded frames
        uint8_t *out_buf[2] = {nullptr, nullptr};
        int out_channels = stereo ? 2 : 1;
        int out_samples = av_rescale_rnd(swr_get_delay(swrCtx, codecContext->sample_rate) + frame->nb_samples,
                                         COMMON_SAMPLE_RATE, codecContext->sample_rate, AV_ROUND_UP);
        av_samples_alloc(out_buf, nullptr, out_channels, out_samples, AV_SAMPLE_FMT_FLT, 0);
        swr_convert(swrCtx, out_buf, out_samples, (const uint8_t **) frame->data, frame->nb_samples);

        int data_size = av_samples_get_buffer_size(nullptr, out_channels, out_samples, AV_SAMPLE_FMT_FLT, 0);
        for (int i = 0; i < data_size / sizeof(float); ++i) {
          pcmf32.push_back(((float *) out_buf[0])[i]);
          if (stereo && out_buf[1] != nullptr) {
            pcmf32s[0].push_back(((float *) out_buf[0])[i]);
            pcmf32s[1].push_back(((float *) out_buf[1])[i]);
          }
        }

        if (out_buf[0]) {
          av_freep(&out_buf[0]);
        }
        if (stereo && out_buf[1]) {
          av_freep(&out_buf[1]);
        }

        av_frame_unref(frame);
      }
      av_packet_unref(&packet);
    }
    av_packet_unref(&packet);
  }

  // Clean up
  av_frame_free(&frame);
  swr_free(&swrCtx);
  avcodec_free_context(&codecContext);
  avformat_close_input(&formatContext);
  avformat_network_deinit();

  return true;
}