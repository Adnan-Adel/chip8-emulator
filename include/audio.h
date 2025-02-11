#ifndef AUDIO_H__
#define AUDIO_H__

#include "config.h"
#include <SDL2/SDL_audio.h>
class Audio {
private:
  SDL_AudioSpec want, have;
  SDL_AudioDeviceID dev;

public:
  Audio(Config &config);
  ~Audio();

  static void MyAudioCallback(void *userdata, Uint8 *stream, int len);

  void play();
  void pause();
  void stop();

};


#endif
