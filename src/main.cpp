#include "../include/chip8.hpp"
#include "../include/display.hpp"
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_path> [options]\n";
        return EXIT_FAILURE;
    }

    std::cout << "Starting CHIP-8 Emulator...\n";

    Config config;
    if (!set_config_from_args(config, argc, argv))
        return EXIT_FAILURE;

    Audio audio(config);
    Display display(config);
    Chip8 chip8(argv[1], audio);

    display.clear_screen(config);

    while (chip8.get_state() != EmulatorState::QUIT) {
        chip8.handle_input(config);

        if (chip8.get_state() == EmulatorState::PAUSED)
            continue;

        const uint64_t frame_start = SDL_GetPerformanceCounter();

        for (uint32_t i = 0; i < config.insts_per_second / 60; ++i)
            chip8.emulate_instruction(config);

        const uint64_t frame_end = SDL_GetPerformanceCounter();
        const double elapsed_ms  = static_cast<double>(frame_end - frame_start) * 1000.0 / static_cast<double>(SDL_GetPerformanceFrequency());

        const double target_ms = 1000.0 / 60.0; // ~16.67 ms
        if (target_ms > elapsed_ms)
            SDL_Delay(static_cast<uint32_t>(target_ms - elapsed_ms));

        if (chip8.get_draw_flag()) {
            display.update_screen(config, chip8);
            chip8.set_draw_flag(false);
        }

        chip8.update_timers();
    }

    return EXIT_SUCCESS;
}
