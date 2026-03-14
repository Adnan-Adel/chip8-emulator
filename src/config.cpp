#include "../include/config.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

// Set up initial emulator configurations from passed in arguments
bool set_config_from_args(Config &config, int argc, char **argv) {

  // Argument parsing
  std::unordered_map<std::string, std::string> args;
  for (int i = 1; i < argc - 1; i++) {
    std::string key = argv[i];
    std::string value = argv[i + 1];

    if (key.rfind("--", 0) == 0) {
      args[key] = value;
      i++; // Skip next as it's already stored as a value
    }
  }

  // Apply arguments to config
  try {
    if (auto it = args.find("--scale-factor"); it != args.end())
      config.scale_factor = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--window-width"); it != args.end())
      config.window_width = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--window-height"); it != args.end())
      config.window_height = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--insts-per-second"); it != args.end())
      config.insts_per_second = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--square-wave-freq"); it != args.end())
      config.square_wave_freq = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--audio-sample-rate"); it != args.end())
      config.audio_sample_rate = static_cast<uint32_t>(std::stoi(it->second));
    if (auto it = args.find("--volume"); it != args.end())
      config.volume = static_cast<int16_t>(std::stoi(it->second));
    if (auto it = args.find("--color-lerp-rate"); it != args.end())
      config.color_lerp_rate = std::stof(it->second);
    if (auto it = args.find("--current-extension"); it != args.end())
      config.current_extension = static_cast<Extension>(std::stoi(it->second));
  }

  catch (const std::exception &e) {
    std::cerr << "Error parsing arguments: " << e.what() << std::endl;
    return false;
  }

  return true;
}
