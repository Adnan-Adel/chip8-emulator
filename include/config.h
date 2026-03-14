#ifndef CONFIG_H__
#define CONFIG_H__

#include <cstdint>

enum Extension { CHIP8, SUPERCHIP, XOCHIP };

struct Config {
  uint32_t window_width = 64;
  uint32_t window_height = 32;
  uint32_t fg_color = 0xFFFFFFFF; // RGBA8888 white
  uint32_t bg_color = 0x000000FF; // RGBA8888 black
  uint32_t scale_factor = 20;
  bool pixel_outlines = true;
  uint32_t insts_per_second = 700;    // CHIP8 CPU clock rate
  uint32_t square_wave_freq = 440;    // 440 Hz middle A
  uint32_t audio_sample_rate = 44100; // CD quality
  int16_t volume = 3000;
  float color_lerp_rate = 0.7f; // Amount to lerp colors by
  Extension current_extension = Extension::CHIP8;
};

// Populates config from argv; returns false on parse error
bool set_config_from_args(Config &config, int argc, char **argv);

#endif
