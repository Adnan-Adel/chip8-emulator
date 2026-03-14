#include "../include/display.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <iostream>

Display::Display(const Config &config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << '\n';
        return;
    }

    window_ = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(config.window_width * config.scale_factor),
        static_cast<int>(config.window_height * config.scale_factor),
        0);

    if (!window_) {
        std::cerr << "Could not create SDL window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "Could not create SDL renderer: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return;
    }

    pixel_color_.fill(config.bg_color);
}

Display::~Display() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void Display::clear_screen(const Config &config) {
    const uint8_t r = (config.bg_color >> 24) & 0xFF;
    const uint8_t g = (config.bg_color >> 16) & 0xFF;
    const uint8_t b = (config.bg_color >> 8) & 0xFF;
    const uint8_t a = (config.bg_color >> 0) & 0xFF;

    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_RenderClear(renderer_);
}

void Display::update_screen(const Config &config, const Chip8 &chip8) {
    SDL_Rect rect{ 0, 0, static_cast<int>(config.scale_factor), static_cast<int>(config.scale_factor) };

    const auto &display = chip8.get_display();

    const uint8_t bg_r = (config.bg_color >> 24) & 0xFF;
    const uint8_t bg_g = (config.bg_color >> 16) & 0xFF;
    const uint8_t bg_b = (config.bg_color >> 8) & 0xFF;
    const uint8_t bg_a = (config.bg_color >> 0) & 0xFF;

    for (std::size_t i = 0; i < display.size(); ++i) {
        rect.x = static_cast<int>((i % config.window_width) * config.scale_factor);
        rect.y = static_cast<int>((i / config.window_width) * config.scale_factor);

        if (display[i]) {
            if (pixel_color_[i] != config.fg_color)
                pixel_color_[i] = color_lerp(pixel_color_[i], config.fg_color, config.color_lerp_rate);

            const uint8_t r = (pixel_color_[i] >> 24) & 0xFF;
            const uint8_t g = (pixel_color_[i] >> 16) & 0xFF;
            const uint8_t b = (pixel_color_[i] >> 8) & 0xFF;
            const uint8_t a = (pixel_color_[i] >> 0) & 0xFF;

            SDL_SetRenderDrawColor(renderer_, r, g, b, a);
            SDL_RenderFillRect(renderer_, &rect);

            if (config.pixel_outlines) {
                SDL_SetRenderDrawColor(renderer_, bg_r, bg_g, bg_b, bg_a);
                SDL_RenderDrawRect(renderer_, &rect);
            }
        } else {
            if (pixel_color_[i] != config.bg_color)
                pixel_color_[i] = color_lerp(pixel_color_[i], config.bg_color, config.color_lerp_rate);

            const uint8_t r = (pixel_color_[i] >> 24) & 0xFF;
            const uint8_t g = (pixel_color_[i] >> 16) & 0xFF;
            const uint8_t b = (pixel_color_[i] >> 8) & 0xFF;
            const uint8_t a = (pixel_color_[i] >> 0) & 0xFF;

            SDL_SetRenderDrawColor(renderer_, r, g, b, a);
            SDL_RenderFillRect(renderer_, &rect);
        }
    }

    SDL_RenderPresent(renderer_);
}

uint32_t Display::color_lerp(uint32_t start_color, uint32_t end_color, float t) {
    const auto lerp_channel = [t](uint8_t s, uint8_t e) -> uint8_t {
        return static_cast<uint8_t>((1.0f - t) * s + t * e);
    };

    const uint8_t r = lerp_channel((start_color >> 24) & 0xFF, (end_color >> 24) & 0xFF);
    const uint8_t g = lerp_channel((start_color >> 16) & 0xFF, (end_color >> 16) & 0xFF);
    const uint8_t b = lerp_channel((start_color >> 8) & 0xFF, (end_color >> 8) & 0xFF);
    const uint8_t a = lerp_channel((start_color >> 0) & 0xFF, (end_color >> 0) & 0xFF);

    return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) | (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);
}
