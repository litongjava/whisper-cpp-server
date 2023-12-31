#ifndef WHISPER_STREAM_COMPONENTS_SERVER_H
#define WHISPER_STREAM_COMPONENTS_SERVER_H

#include <vector>
#include <whisper.h>
#include "stream_components_params.h"
#include "stream_components_output.h"

using std::shared_ptr;

namespace stream_components
{

  /**
   * Encapsulates the Whisper service.
   */
  class WhisperService
  {
  public:
    WhisperService(
        const struct service_params &sparams,
        const struct audio_params &aparams,
        const struct whisper_context_params &cparams);

    ~WhisperService();

    bool process(const float *samples, int size);

    service_params sparams;
    audio_params aparams;

    struct whisper_context *ctx;

  protected:
    std::vector<whisper_token> prompt_tokens;
  };

} // namespace stream_components

#endif // WHISPER_STREAM_COMPONENTS_SERVER_H
