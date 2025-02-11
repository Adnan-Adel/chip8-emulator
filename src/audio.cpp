#include "../include/audio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cstdint>

Audio::Audio(Config &config) {
  if(SDL_Init(SDL_INIT_AUDIO) != 0) {
    SDL_Log("Could not initialize SDL AUDIO! %s\n", SDL_GetError());
    return;
  }
  
  want.freq = 44100;         // CD-quality audio (44.1 kHz)
  want.format = AUDIO_S16LSB; // 16-bit signed samples, little-endian
  want.channels = 1;         // Mono
  want.samples = 512;
  want.callback = MyAudioCallback;
  want.userdata = &config;   // Can pass an object reference later
  
  // Open audio device
  dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
  if(dev == 0) {
    SDL_Log("Could not get an audio device! %s\n", SDL_GetError());
  }
  
  // Check if requested format was granted
  if((want.format != have.format) ||
     (want.channels != have.channels)) {
    SDL_Log("Could not get desired audio spec! %s\n", SDL_GetError());
  }
}

Audio::~Audio() {
  if (dev) {
    SDL_CloseAudioDevice(dev);
  }
  SDL_Quit();
}

void Audio::MyAudioCallback(void *userdata, Uint8 *stream, int len) {
  Config *config = (Config *)userdata;
  
  int16_t *audio_data = (int16_t *)stream;
  static uint32_t running_sample_index = 0;
  const int32_t square_wave_period = config->audio_sample_rate / config->square_wave_freq;
  const int32_t half_square_wave_period = square_wave_period / 2;
  
  for(int i = 0; i < len / 2; i++) {
    audio_data[i] = ((running_sample_index++ / half_square_wave_period) % 2) ? 
                    config->volume : -config->volume;
  }

}

void Audio::play() {
  if (dev) {
    SDL_PauseAudioDevice(dev, 0); // Unpause audio
  }
}

// stop function
void Audio::stop() {
  if (dev) {
    SDL_PauseAudioDevice(dev, 1); // Pause audio
  }
}

