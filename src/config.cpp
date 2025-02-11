#include "../include/config.h"
#include <iostream>
#include <string>
#include <unordered_map>

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
    .insts_per_seconds = 700,     // Number of instructions to emulate in 1 sec 
    .square_wave_freq = 440,      // 440hz for middle A
    .audio_sample_rate = 44100,   // CD quality, 44100 hz
    .volume = 3000,               
    .color_lerp_rate = 0.7,
  };

 // Argument parsing
  std::unordered_map<std::string, std::string> args;
  for(int i = 1; i < argc - 1; i++) { 
    std::string key = argv[i];
    std::string value = argv[i + 1];
    
    if (key.rfind("--", 0) == 0) {
      args[key] = value;
      i++; // Skip next as it's already stored as a value
    }
  }

  // Apply arguments to config
  try {
    if (args.find("--scale-factor") != args.end()) {
      config->scale_factor = std::stoi(args["--scale-factor"]);
    }
    if (args.find("--window-width") != args.end()) {
      config->window_width = std::stoi(args["--window-width"]);
    }
    if (args.find("--window-height") != args.end()) {
      config->window_height = std::stoi(args["--window-height"]);
    }
    if (args.find("--insts-per-second") != args.end()) {
      config->insts_per_seconds = std::stoi(args["--insts-per-second"]);
    }
    if (args.find("--square-wave-freq") != args.end()) {
      config->square_wave_freq = std::stoi(args["--square-wave-freq"]);
    }
    if (args.find("--audio-sample-rate") != args.end()) {
      config->audio_sample_rate = std::stoi(args["--audio-sample-rate"]);
    }
    if (args.find("--volume") != args.end()) {
      config->volume = std::stoi(args["--volume"]);
    }
    if (args.find("--color-lerp-rate") != args.end()) {
      config->color_lerp_rate = std::stof(args["--color-lerp-rate"]);
    }
  } catch (const std::exception &e) {
    std::cerr << "Error parsing arguments: " << e.what() << std::endl;
    return false;
  }

  return true;
}
