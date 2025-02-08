#include <SDL2/SDL_timer.h>
#include <cstdio>
#include <cstdlib>
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
  display.update_screen(&config, chip8);
  
  // main emulator loop
  while (chip8.get_state() != EmulatorState::QUIT) {
    // handle user input
    chip8.handle_input();
    
    if(chip8.get_state() == PAUSED)
      continue;

    // get_time();
    
    // Emulate CHIP8 Instructions
    chip8.emulate_instruction(config);

    // get_time() elapsed since last get_time();

    // delay for approximately 60hz/60fps (16.67ms)
    // SDL_Delay(16 - actual_time_elapsed);
    SDL_Delay(16);

    // update window with changes
    display.update_screen(&config, chip8);
  }

  return 0;
}
