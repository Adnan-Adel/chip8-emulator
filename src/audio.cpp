#include "../include/audio.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cstdint>
#include <iostream>

Audio::Audio(const Config &config) {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "Could not initialize SDL audio: " << SDL_GetError() << '\n';
        return;
    }

    want_.freq = static_cast<int>(config.audio_sample_rate);
    want_.format = AUDIO_S16LSB;
    want_.channels = 1;
    want_.samples = 512;
    want_.callback = audioCallback;
    want_.userdata =
        const_cast<Config *>(&config); // config lifetime outlives Audio

    // Open audio device
    dev_ = SDL_OpenAudioDevice(nullptr, 0, &want_, &have_, 0);
    if (dev_ == 0) {
        std::cerr << "Could not open audio device: " << SDL_GetError() << '\n';
        return;
    }

    if (want_.format != have_.format || want_.channels != have_.channels) {
        std::cerr << "Audio device did not grant requested format.\n";
    }
}

Audio::~Audio() {
    if (dev_) {
        SDL_CloseAudioDevice(dev_);
    }
    SDL_Quit();
}

void Audio::audioCallback(void *userdata, Uint8 *stream, int len) {
    const Config *config = static_cast<const Config *>(userdata);

    auto *audio_data = reinterpret_cast<int16_t *>(stream);
    static uint32_t running_sample_index = 0;

    const int32_t period = static_cast<int32_t>(config->audio_sample_rate /
                                                config->square_wave_freq);
    const int32_t half_period = period / 2;

    const int sample_count = len / 2;
    for (int i = 0; i < sample_count; ++i) {
        audio_data[i] = ((running_sample_index++ / half_period) % 2)
                            ? config->volume
                            : static_cast<int16_t>(-config->volume);
    }
}

void Audio::play() {
    if (dev_) {
        SDL_PauseAudioDevice(dev_, 0); // Unpause audio
    }
}

// stop function
void Audio::stop() {
    if (dev_) {
        SDL_PauseAudioDevice(dev_, 1); // Pause audio
    }
}
