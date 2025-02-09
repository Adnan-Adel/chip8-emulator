#include "../include/config.h"
#include <iostream>

// Set up initial emulator configurations from passed in arguments
bool set_config_from_args(Config *config, int argc, char **argv) {
  // Set defaults
  *config = {
    .window_width  = 64,         // CHIP-8 original X resolution
    .window_height = 32,         // CHIP-8 original Y resolution
    .fg_color      = 0xFFFFFFFF, // WHITE
    .bg_color      = 0x000000FF, // BLACK
    .scale_factor  = 20,         // Scale multiplier
    .pixel_outlines = true,
    .insts_per_seconds = 500     // Number of instructions to emulate in 1 sec 
  };

  // Override defaults from passed in arguments (TODO: Parse actual arguments)
  for (int i = 1; i < argc; i++) {
    std::cout << "Ignoring argument: " << argv[i] << std::endl;
  }

  return true;
}

