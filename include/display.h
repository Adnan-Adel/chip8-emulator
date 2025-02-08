#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "SDL2/SDL.h"
#include "chip8.h"
#include "config.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>


class Display {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;

public:
  Display();
  Display(Config *conf);
  ~Display();
  void clear_screen(const Config *conf);
  void update_screen(const Config *config, const Chip8 &chip8);

};


#endif
