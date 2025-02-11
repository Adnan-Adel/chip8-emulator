#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "SDL2/SDL.h"
#include "chip8.h"
#include "config.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdint>


class Display {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::array<uint32_t, 64 * 32> pixel_color{};   // pixel colors to draw

public:
  Display();
  Display(Config *conf);
  ~Display();
  void clear_screen(const Config *conf);
  void update_screen(const Config *config, const Chip8 &chip8);
  uint32_t color_lerp(const uint32_t start_color, const uint32_t end_color, const float t); 
};


#endif
