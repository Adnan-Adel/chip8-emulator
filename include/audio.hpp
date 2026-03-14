#ifndef AUDIO_H__
#define AUDIO_H__

#include "config.hpp"
#include <SDL2/SDL_audio.h>
class Audio {
public:
    explicit Audio(const Config &config);
    ~Audio();

    // Non-copyable
    Audio(const Audio &) = delete;
    Audio &operator=(const Audio &) = delete;

    void play();
    void stop();

private:
    SDL_AudioSpec want_{};
    SDL_AudioSpec have_{};
    SDL_AudioDeviceID dev_ = 0;

    static void audioCallback(void *userdata, Uint8 *stream, int len);
};

#endif
