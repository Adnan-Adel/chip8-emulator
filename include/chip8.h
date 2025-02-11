#ifndef CHIP8_H__
#define CHIP8_H__

#include "audio.h"
#include "config.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
enum EmulatorState {
  QUIT,
  RUNNING,
  PAUSED
};

struct Instruction {
  uint16_t opcode;
  uint16_t NNN;             // 12 bit address/constant
  uint8_t NN;               // 8 bit constant
  uint8_t N;                // 4 bit constant
  uint8_t X;                // 4 bit register identifier
  uint8_t Y;                // 4 bit register identifier
};

class Chip8 {
private:
  static constexpr size_t RAM_SIZE = 4096;
  static constexpr size_t SCREEN_WIDTH = 64;
  static constexpr size_t SCREEN_HEIGHT = 32;
  static constexpr size_t STACK_SIZE = 12;
  static constexpr uint16_t ROM_START = 0x200;

  EmulatorState state = EmulatorState::RUNNING;
  std::array<uint8_t, RAM_SIZE> ram{}; 
  std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT> display{};   // emulate original CHIP8 pixels
  std::array<uint16_t, STACK_SIZE> stack{};                   // subroutine stack (12 levels of nesting)
  uint16_t *stack_ptr = nullptr;

  std::array<uint8_t, 16> V{};    // Data registers V0 - VF
  uint16_t I = 0;                 // Index register 
  uint16_t PC = ROM_START;        // Program Counter
  uint8_t delay_timer = 0;        // decrements at 60hz when > 0
  uint8_t sound_timer = 0;        // decrements at 60hz when > 0
  std::array<bool, 16> keypad{};

  std::string rom_name;           // currently running ROM
  Instruction inst;               // currently executing instructiom
  
  bool draw;                      // update the screen flag

  Audio *audio;

  void load_rom(const std::string &rom_path);
  void load_fontset();

public:
  explicit Chip8(const std::string &rom_path, Audio *audio_ref);

  void run();
  void handle_input(Config &config);
  EmulatorState get_state() const {return state; }
  void emulate_instruction(const Config &config);
  void print_debug_info() const;
  void update_timers();
  void reset();
  
  const std::array<bool, SCREEN_WIDTH * SCREEN_HEIGHT>& get_display() const {
    return display;
  }


  bool get_draw_flag() const { return draw; }
  void set_draw_flag(bool value) { draw = value; }
};


#endif
