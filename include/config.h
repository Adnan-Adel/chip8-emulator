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
};

// Function to set up initial emulator configurations from passed arguments
bool set_config_from_args(Config *config, int argc, char **argv);

#endif
