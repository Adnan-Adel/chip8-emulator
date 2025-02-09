#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "../include/chip8.h"
#include "../include/display.h"
using namespace std;

int main(int argc, char **argv) {
  // Default Usage message for args
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  cout << "Starting Chip-8 Emulator...." << "\n";
 
  // Initialize Emulator Configs
  Config config;
  if (!set_config_from_args(&config, argc, argv)) {
    exit(EXIT_FAILURE);
  }

  Display display(&config);

  const char *rom_name = argv[1];
  Chip8 chip8(rom_name);

  display.clear_screen(&config);
  
  // Seed the random number generator
  srand(time(nullptr));

  // main emulator loop
  while (chip8.get_state() != EmulatorState::QUIT) {
    // handle user input
    chip8.handle_input();
    
    if(chip8.get_state() == PAUSED)
      continue;

    // Get time before running instructions
    const uint64_t start_frame_time = SDL_GetPerformanceCounter();
    
    // Emulate CHIP8 Instructions for this emulator "frame" (60hz)
    for(uint32_t i = 0; i < config.insts_per_seconds / 60; i++)
      chip8.emulate_instruction(config);

    // Get time elapsed after running instructions
    const uint64_t end_frame_time = SDL_GetPerformanceCounter();

    // delay for approximately 60hz/60fps (16.67ms) or actual time
    const double time_elapsed = ((double)(end_frame_time - start_frame_time) / 1000) / SDL_GetPerformanceFrequency();
    
    // SDL_Delay(16 - actual_time_elapsed);
    SDL_Delay(16.67f > time_elapsed ? 16.67f - time_elapsed : 0);

    // update window with changes every 60hz
    display.update_screen(&config, chip8);
    chip8.update_timers();

    // update delay & sound timers every 60hz
  }

  return 0;
}
