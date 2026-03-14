#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "chip8.hpp"
#include "config.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdint>

class Display {
public:
    explicit Display(const Config &config);
    ~Display();

    // Non-copyable
    Display(const Display &)            = delete;
    Display &operator=(const Display &) = delete;

    void clear_screen(const Config &config);
    void update_screen(const Config &config, const Chip8 &chip8);

private:
    SDL_Window *window_     = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    std::array<uint32_t, 64 * 32> pixel_color_{};

    static uint32_t color_lerp(uint32_t start_color, uint32_t end_color, float t);
};

#endif
