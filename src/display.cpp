#include "../include/display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <cstdint>

Display::Display(Config *conf) {
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    SDL_Log("Could not initialize SDL! %s\n", SDL_GetError());
    return;
  }
  
  // setup window
  window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            conf->window_width * conf->scale_factor,
                            conf->window_height * conf->scale_factor,
                            0);
  
  if(!window) {
    SDL_Log("Couldn't create SDL window %s\n", SDL_GetError());
    SDL_Quit();  // Free SDL resources
    return;
  }

  // create a renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    SDL_Log("Couldn't create SDL renderer %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }
}

Display::~Display() {
  if(renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if(window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  SDL_Quit();
}

void Display::clear_screen(const Config *conf) {
    // extract R, G, B, and A
  const uint8_t r = (uint8_t)(conf->bg_color >> 24) & 0xFF;
  const uint8_t g = (uint8_t)(conf->bg_color >> 16) & 0xFF;
  const uint8_t b = (uint8_t)(conf->bg_color >> 8)  & 0xFF;
  const uint8_t a = (uint8_t)(conf->bg_color >> 0)  & 0xFF;

  SDL_SetRenderDrawColor(renderer, r, g, b, a); // Black background
  SDL_RenderClear(renderer);
}

void Display::update_screen(const Config *config, const Chip8 &chip8) {
  SDL_Rect rect = {.x = 0, .y = 0, .w = (int)config->scale_factor, .h = (int)config->scale_factor};
  
  const auto& display = chip8.get_display(); 
  
  // grab color values to draw
  const uint8_t fg_r = (uint8_t)(config->fg_color >> 24) & 0xFF;
  const uint8_t fg_g = (uint8_t)(config->fg_color >> 16) & 0xFF;
  const uint8_t fg_b = (uint8_t)(config->fg_color >> 8)  & 0xFF;
  const uint8_t fg_a = (uint8_t)(config->fg_color >> 0)  & 0xFF;
   
  const uint8_t bg_r = (uint8_t)(config->bg_color >> 24) & 0xFF;
  const uint8_t bg_g = (uint8_t)(config->bg_color >> 16) & 0xFF;
  const uint8_t bg_b = (uint8_t)(config->bg_color >> 8)  & 0xFF;
  const uint8_t bg_a = (uint8_t)(config->bg_color >> 0)  & 0xFF;
  
  // loop through display pixels, draw rectangle per pixel to the SDL window
  for(size_t i = 0; i < sizeof display; i++) {
    // translate 1D index i to 2D X/Y coordinates
    rect.x = (i % config->window_width) * config->scale_factor;
    rect.y = (i / config->window_width) * config->scale_factor;

    if(display[i]) {
      // pixel is on, draw foreground color
      SDL_SetRenderDrawColor(renderer, fg_r, fg_g, fg_b, fg_a);
      SDL_RenderFillRect(renderer, &rect);
      
      // if user requested drawing pixel outlines, draw those here
      if(config->pixel_outlines) {
      SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
      SDL_RenderDrawRect(renderer, &rect);
      }
    }
    else {
      // pixel is off, draw background color
      SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
}
