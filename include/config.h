#ifndef CONFIG_H__
#define CONFIG_H__


#include <cstdint>
struct Config {
  uint32_t window_width;
  uint32_t window_height;
  uint32_t fg_color;            // foreground color RGBA8888
  uint32_t bg_color;            // background color RGBA8888
  uint32_t scale_factor;        // amout to scale a chip-8 pixel by
  bool pixel_outlines;
  uint32_t insts_per_seconds;   // CHIP8 CPU clock rate
  uint32_t square_wave_freq;    // Frequency of square wave sound
  uint32_t audio_sample_rate;
  int16_t volume;               // How loud or not is the sound 
  float color_lerp_rate;              // Amount to lerp colors by
};
 
// Function to set up initial emulator configurations from passed arguments
bool set_config_from_args(Config *config, int argc, char **argv);

#endif
