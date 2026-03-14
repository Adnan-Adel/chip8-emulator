#ifndef CHIP8_H__
#define CHIP8_H__

#include "audio.hpp"
#include "config.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

enum class EmulatorState {
    QUIT,
    RUNNING,
    PAUSED
};

struct Instruction {
    uint16_t opcode = 0;
    uint16_t NNN    = 0; // 12-bit address / constant
    uint8_t NN      = 0; // 8-bit constant
    uint8_t N       = 0; // 4-bit constant
    uint8_t X       = 0; // 4-bit register index
    uint8_t Y       = 0; // 4-bit register index
};

class Chip8 {
public:
    explicit Chip8(const std::string &rom_path, Audio &audio);

    // Main interface
    void emulate_instruction(const Config &config);
    void handle_input(Config &config);
    void update_timers();
    void reset();

    // Accessors
    EmulatorState get_state() const { return state_; }
    bool get_draw_flag() const { return draw_; }
    void set_draw_flag(bool v) { draw_ = v; }

    const std::array<bool, 64 * 32> &get_display() const { return display_; }

#ifdef DEBUG
    void print_debug_info() const;
#endif

private:
    static constexpr std::size_t RAM_SIZE   = 4096;
    static constexpr std::size_t SCREEN_W   = 64;
    static constexpr std::size_t SCREEN_H   = 32;
    static constexpr std::size_t STACK_SIZE = 16;
    static constexpr uint16_t ROM_START     = 0x200;

    // State
    EmulatorState state_ = EmulatorState::RUNNING;

    // Memory & display
    std::array<uint8_t, RAM_SIZE> ram_{};
    std::array<bool, SCREEN_W * SCREEN_H> display_{};

    // Stack — managed with an index, not a raw pointer
    std::array<uint16_t, STACK_SIZE> stack_{};
    uint8_t sp_ = 0; // stack pointer index

    // Registers
    std::array<uint8_t, 16> V_{};
    uint16_t I_  = 0;
    uint16_t PC_ = ROM_START;

    // Timers
    uint8_t delay_timer_ = 0;
    uint8_t sound_timer_ = 0;

    // Input
    std::array<bool, 16> keypad_{};

    // Meta
    std::string rom_name_;
    Instruction inst_{};
    bool draw_ = false;
    Audio &audio_;

    // FX0A wait-for-key state
    bool fx0a_waiting_ = false;
    uint8_t fx0a_key_  = 0xFF;

    // RNG
    std::mt19937 rng_{ std::random_device{}() };
    std::uniform_int_distribution<int> rand_byte_{ 0, 255 };

    void load_rom(const std::string &rom_path);
    void load_fontset();
};

#endif
