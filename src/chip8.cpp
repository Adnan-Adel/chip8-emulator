#include "../include/chip8.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>

// ---------------------------------------------------------------------------
// Fontset
// ---------------------------------------------------------------------------
// clang-format off
static constexpr std::array<uint8_t, 80> FONTSET = {{
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
}};
// clang-format on

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
Chip8::Chip8(const std::string &rom_path, Audio &audio)
    : rom_name_(rom_path), audio_(audio) {
    load_fontset();
    load_rom(rom_path);
}

void Chip8::load_fontset() {
    std::copy(FONTSET.begin(), FONTSET.end(), ram_.begin());
}

void Chip8::load_rom(const std::string &rom_path) {
    std::ifstream rom(rom_path, std::ios::binary | std::ios::ate);
    if (!rom) {
        std::cerr << "Error: ROM \"" << rom_path << "\" is invalid or does not exist.\n";
        state_ = EmulatorState::QUIT;
        return;
    }

    const auto rom_size            = static_cast<std::size_t>(rom.tellg());
    constexpr std::size_t max_size = RAM_SIZE - ROM_START;
    if (rom_size > max_size) {
        std::cerr << "Error: ROM \"" << rom_path << "\" is too large ("
                  << rom_size << " bytes; max " << max_size << ").\n";
        state_ = EmulatorState::QUIT;
        return;
    }

    rom.seekg(0, std::ios::beg);
    rom.read(reinterpret_cast<char *>(&ram_[ROM_START]), static_cast<std::streamsize>(rom_size));
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
void Chip8::handle_input(Config &config) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {

            case SDL_QUIT:
                state_ = EmulatorState::QUIT;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        state_ = EmulatorState::QUIT;
                        break;

                    case SDLK_SPACE:
                        if (state_ == EmulatorState::RUNNING) {
                            state_ = EmulatorState::PAUSED;
                            std::cout << "========= PAUSED =========\n";
                        } else {
                            state_ = EmulatorState::RUNNING;
                            std::cout << "========= RUNNING =========\n";
                        }
                        break;

                    case SDLK_EQUALS:
                        reset();
                        break;

                    case SDLK_j:
                        if (config.color_lerp_rate > 0.1f) config.color_lerp_rate -= 0.1f;
                        break;

                    case SDLK_k:
                        if (config.color_lerp_rate < 1.0f) config.color_lerp_rate += 0.1f;
                        break;

                    case SDLK_o:
                        if (config.volume > 0) config.volume -= 500;
                        break;

                    case SDLK_p:
                        if (config.volume < INT16_MAX) config.volume += 500;
                        break;

                    // CHIP-8 keypad (QWERTY layout)
                    case SDLK_1: keypad_[0x1] = true; break;
                    case SDLK_2: keypad_[0x2] = true; break;
                    case SDLK_3: keypad_[0x3] = true; break;
                    case SDLK_4: keypad_[0xC] = true; break;

                    case SDLK_q: keypad_[0x4] = true; break;
                    case SDLK_w: keypad_[0x5] = true; break;
                    case SDLK_e: keypad_[0x6] = true; break;
                    case SDLK_r: keypad_[0xD] = true; break;

                    case SDLK_a: keypad_[0x7] = true; break;
                    case SDLK_s: keypad_[0x8] = true; break;
                    case SDLK_d: keypad_[0x9] = true; break;
                    case SDLK_f: keypad_[0xE] = true; break;

                    case SDLK_z: keypad_[0xA] = true; break;
                    case SDLK_x: keypad_[0x0] = true; break;
                    case SDLK_c: keypad_[0xB] = true; break;
                    case SDLK_v: keypad_[0xF] = true; break;

                    default: break;
                }
                break; // SDL_KEYDOWN

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_1: keypad_[0x1] = false; break;
                    case SDLK_2: keypad_[0x2] = false; break;
                    case SDLK_3: keypad_[0x3] = false; break;
                    case SDLK_4: keypad_[0xC] = false; break;

                    case SDLK_q: keypad_[0x4] = false; break;
                    case SDLK_w: keypad_[0x5] = false; break;
                    case SDLK_e: keypad_[0x6] = false; break;
                    case SDLK_r: keypad_[0xD] = false; break;

                    case SDLK_a: keypad_[0x7] = false; break;
                    case SDLK_s: keypad_[0x8] = false; break;
                    case SDLK_d: keypad_[0x9] = false; break;
                    case SDLK_f: keypad_[0xE] = false; break;

                    case SDLK_z: keypad_[0xA] = false; break;
                    case SDLK_x: keypad_[0x0] = false; break;
                    case SDLK_c: keypad_[0xB] = false; break;
                    case SDLK_v: keypad_[0xF] = false; break;

                    default: break;
                }
                break; // SDL_KEYUP

            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// Timers
// ---------------------------------------------------------------------------
void Chip8::update_timers() {
    if (delay_timer_ > 0) --delay_timer_;

    if (sound_timer_ > 0) {
        --sound_timer_;
        audio_.play();
    } else {
        audio_.stop();
    }
}

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
void Chip8::reset() {
    ram_.fill(0);
    display_.fill(false);
    stack_.fill(0);
    keypad_.fill(false);
    V_.fill(0);

    I_           = 0;
    PC_          = ROM_START;
    sp_          = 0;
    delay_timer_ = 0;
    sound_timer_ = 0;
    draw_        = true;

    // FX0A state must also be reset or re-waiting after reset is a bug
    fx0a_waiting_ = false;
    fx0a_key_     = 0xFF;

    // Fontset must be reloaded — ram was just zeroed
    load_fontset();

    if (!rom_name_.empty())
        load_rom(rom_name_);

    std::cout << "========= CHIP-8 RESET =========\n";
}

// ---------------------------------------------------------------------------
// Debug
// ---------------------------------------------------------------------------
#ifdef DEBUG
void Chip8::print_debug_info() const {
    // Use std::cout throughout — no printf mixing
    std::cout << std::hex << std::uppercase;
    std::cout << "Address: 0x" << (PC_ - 2)
              << "  Opcode: 0x" << inst_.opcode
              << "  Desc: ";

    switch ((inst_.opcode >> 12) & 0x0F) {
        case 0x00:
            if (inst_.NN == 0xE0)
                std::cout << "Clear screen";
            else if (inst_.NN == 0xEE)
                std::cout << "Return from subroutine to 0x" << stack_[sp_ - 1];
            break;
        case 0x01: std::cout << "Jump to 0x" << inst_.NNN; break;
        case 0x02: std::cout << "Call subroutine 0x" << inst_.NNN; break;
        case 0x03: std::cout << "Skip if V" << +inst_.X << " == 0x" << +inst_.NN; break;
        case 0x04: std::cout << "Skip if V" << +inst_.X << " != 0x" << +inst_.NN; break;
        case 0x05: std::cout << "Skip if V" << +inst_.X << " == V" << +inst_.Y; break;
        case 0x06: std::cout << "V" << +inst_.X << " = 0x" << +inst_.NN; break;
        case 0x07: std::cout << "V" << +inst_.X << " += 0x" << +inst_.NN; break;
        case 0x08:
            switch (inst_.N) {
                case 0x0: std::cout << "V" << +inst_.X << " = V" << +inst_.Y; break;
                case 0x1: std::cout << "V" << +inst_.X << " |= V" << +inst_.Y; break;
                case 0x2: std::cout << "V" << +inst_.X << " &= V" << +inst_.Y; break;
                case 0x3: std::cout << "V" << +inst_.X << " ^= V" << +inst_.Y; break;
                case 0x4: std::cout << "V" << +inst_.X << " += V" << +inst_.Y << " (carry->VF)"; break;
                case 0x5: std::cout << "V" << +inst_.X << " -= V" << +inst_.Y << " (borrow->VF)"; break;
                case 0x6: std::cout << "V" << +inst_.X << " >>= 1 (bit->VF)"; break;
                case 0x7: std::cout << "V" << +inst_.X << " = V" << +inst_.Y << " - V" << +inst_.X; break;
                case 0xE: std::cout << "V" << +inst_.X << " <<= 1 (bit->VF)"; break;
                default: std::cout << "Unknown 8XYN (N=0x" << +inst_.N << ")"; break;
            }
            break;
        case 0x09: std::cout << "Skip if V" << +inst_.X << " != V" << +inst_.Y; break;
        case 0x0A: std::cout << "I = 0x" << inst_.NNN; break;
        case 0x0B: std::cout << "PC = 0x" << inst_.NNN << " + V0"; break;
        case 0x0C: std::cout << "V" << +inst_.X << " = rand & 0x" << +inst_.NN; break;
        case 0x0D: std::cout << "Draw " << +inst_.N << " rows at V" << +inst_.X << ",V" << +inst_.Y; break;
        case 0x0E:
            if (inst_.NN == 0x9E)
                std::cout << "Skip if key V" << +inst_.X << " pressed";
            else if (inst_.NN == 0xA1)
                std::cout << "Skip if key V" << +inst_.X << " not pressed";
            break;
        case 0x0F:
            switch (inst_.NN) {
                case 0x07: std::cout << "V" << +inst_.X << " = delay_timer"; break;
                case 0x0A: std::cout << "Wait for key -> V" << +inst_.X; break;
                case 0x15: std::cout << "delay_timer = V" << +inst_.X; break;
                case 0x18: std::cout << "sound_timer = V" << +inst_.X; break;
                case 0x1E: std::cout << "I += V" << +inst_.X; break;
                case 0x29: std::cout << "I = sprite addr for V" << +inst_.X; break;
                case 0x33: std::cout << "BCD(V" << +inst_.X << ") -> [I]"; break;
                case 0x55: std::cout << "Dump V0-V" << +inst_.X << " to [I]"; break;
                case 0x65: std::cout << "Load V0-V" << +inst_.X << " from [I]"; break;
                default: std::cout << "Unknown FX (NN=0x" << +inst_.NN << ")"; break;
            }
            break;
        default:
            std::cout << "Unimplemented opcode 0x" << inst_.opcode;
            break;
    }

    std::cout << std::dec << '\n';
}
#endif // DEBUG

// ---------------------------------------------------------------------------
// Emulate one instruction
// ---------------------------------------------------------------------------
void Chip8::emulate_instruction(const Config &config) {
    // Fetch
    inst_.opcode = static_cast<uint16_t>((ram_[PC_] << 8) | ram_[PC_ + 1]);
    PC_ += 2;

    // Decode
    inst_.NNN = inst_.opcode & 0x0FFF;
    inst_.NN  = inst_.opcode & 0x00FF;
    inst_.N   = inst_.opcode & 0x000F;
    inst_.X   = (inst_.opcode >> 8) & 0x0F;
    inst_.Y   = (inst_.opcode >> 4) & 0x0F;

#ifdef DEBUG
    print_debug_info();
#endif

    // Execute
    switch ((inst_.opcode >> 12) & 0x0F) {

        case 0x00:
            if (inst_.NN == 0xE0) {
                // 00E0: Clear screen
                display_.fill(false);
                draw_ = true;
            } else if (inst_.NN == 0xEE) {
                // 00EE: Return from subroutine
                assert(sp_ > 0 && "Stack underflow");
                PC_ = stack_[--sp_];
            }
            break;

        case 0x01:
            // 1NNN: Jump
            PC_ = inst_.NNN;
            break;

        case 0x02:
            // 2NNN: Call subroutine
            assert(sp_ < STACK_SIZE && "Stack overflow");
            stack_[sp_++] = PC_;
            PC_           = inst_.NNN;
            break;

        case 0x03:
            // 3XNN: Skip if VX == NN
            if (V_[inst_.X] == inst_.NN) PC_ += 2;
            break;

        case 0x04:
            // 4XNN: Skip if VX != NN
            if (V_[inst_.X] != inst_.NN) PC_ += 2;
            break;

        case 0x05:
            // 5XY0: Skip if VX == VY
            if (inst_.N != 0) break; // invalid sub-opcode
            if (V_[inst_.X] == V_[inst_.Y]) PC_ += 2;
            break;

        case 0x06:
            // 6XNN: VX = NN
            V_[inst_.X] = inst_.NN;
            break;

        case 0x07:
            // 7XNN: VX += NN (no carry flag)
            V_[inst_.X] += inst_.NN;
            break;

        case 0x08: {
            const bool is_chip8 = (config.current_extension == Extension::CHIP8);
            switch (inst_.N) {
                case 0x0:
                    // 8XY0: VX = VY
                    V_[inst_.X] = V_[inst_.Y];
                    break;
                case 0x1:
                    // 8XY1: VX |= VY
                    V_[inst_.X] |= V_[inst_.Y];
                    if (is_chip8) V_[0xF] = 0;
                    break;
                case 0x2:
                    // 8XY2: VX &= VY
                    V_[inst_.X] &= V_[inst_.Y];
                    if (is_chip8) V_[0xF] = 0;
                    break;
                case 0x3:
                    // 8XY3: VX ^= VY
                    V_[inst_.X] ^= V_[inst_.Y];
                    if (is_chip8) V_[0xF] = 0;
                    break;
                case 0x4: {
                    // 8XY4: VX += VY, VF = carry
                    const uint16_t sum = static_cast<uint16_t>(V_[inst_.X]) + V_[inst_.Y];
                    V_[inst_.X]        = static_cast<uint8_t>(sum);
                    V_[0xF]            = (sum > 0xFF) ? 1 : 0;
                    break;
                }
                case 0x5: {
                    // 8XY5: VX -= VY, VF = !borrow
                    const uint8_t vx = V_[inst_.X];
                    const uint8_t vy = V_[inst_.Y];
                    V_[inst_.X]      = vx - vy;
                    V_[0xF]          = (vx >= vy) ? 1 : 0;
                    break;
                }
                case 0x6:
                    // 8XY6: VX >>= 1 (CHIP8: use VY; SCHIP: use VX)
                    if (is_chip8) {
                        V_[0xF]     = V_[inst_.Y] & 0x01;
                        V_[inst_.X] = V_[inst_.Y] >> 1;
                    } else {
                        V_[0xF] = V_[inst_.X] & 0x01;
                        V_[inst_.X] >>= 1;
                    }
                    break;
                case 0x7: {
                    // 8XY7: VX = VY - VX, VF = !borrow
                    const uint8_t vx = V_[inst_.X];
                    const uint8_t vy = V_[inst_.Y];
                    V_[inst_.X]      = vy - vx;
                    V_[0xF]          = (vy >= vx) ? 1 : 0;
                    break;
                }
                case 0xE:
                    // 8XYE: VX <<= 1 (CHIP8: use VY; SCHIP: use VX)
                    if (is_chip8) {
                        V_[0xF]     = (V_[inst_.Y] & 0x80) >> 7;
                        V_[inst_.X] = V_[inst_.Y] << 1;
                    } else {
                        V_[0xF] = (V_[inst_.X] & 0x80) >> 7;
                        V_[inst_.X] <<= 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        }

        case 0x09:
            // 9XY0: Skip if VX != VY
            if (V_[inst_.X] != V_[inst_.Y]) PC_ += 2;
            break;

        case 0x0A:
            // ANNN: I = NNN
            I_ = inst_.NNN;
            break;

        case 0x0B:
            // BNNN: PC = NNN + V0
            PC_ = inst_.NNN + V_[0];
            break;

        case 0x0C:
            // CXNN: VX = rand() & NN
            V_[inst_.X] = static_cast<uint8_t>(rand_byte_(rng_)) & inst_.NN;
            break;

        case 0x0D: {
            // DXYN: Draw N-row sprite at (VX, VY)
            const uint8_t x_start = V_[inst_.X] % static_cast<uint8_t>(config.window_width);
            const uint8_t y_start = V_[inst_.Y] % static_cast<uint8_t>(config.window_height);
            V_[0xF]               = 0;

            for (uint8_t row = 0; row < inst_.N; ++row) {
                const uint8_t sprite_byte = ram_[I_ + row];
                const uint8_t y           = y_start + row;
                if (y >= config.window_height) break;

                for (int8_t col = 7; col >= 0; --col) {
                    const uint8_t x = x_start + static_cast<uint8_t>(7 - col);
                    if (x >= config.window_width) break;

                    const bool sprite_bit = (sprite_byte >> col) & 0x01;
                    bool &pixel           = display_[y * config.window_width + x];

                    if (sprite_bit && pixel) V_[0xF] = 1;
                    pixel ^= sprite_bit;
                }
            }
            draw_ = true;
            break;
        }

        case 0x0E:
            if (inst_.NN == 0x9E) {
                // EX9E: Skip if key VX pressed
                if (keypad_[V_[inst_.X]]) PC_ += 2;
            } else if (inst_.NN == 0xA1) {
                // EXA1: Skip if key VX not pressed
                if (!keypad_[V_[inst_.X]]) PC_ += 2;
            }
            break;

        case 0x0F:
            switch (inst_.NN) {

                case 0x07:
                    // FX07: VX = delay_timer
                    V_[inst_.X] = delay_timer_;
                    break;

                case 0x0A: {
                    // FX0A: Wait for key press, store in VX
                    // State is tracked in members, not statics
                    if (!fx0a_waiting_) {
                        // Phase 1: scan for any key currently pressed
                        for (uint8_t i = 0; i < static_cast<uint8_t>(keypad_.size()); ++i) {
                            if (keypad_[i]) {
                                fx0a_key_     = i;
                                fx0a_waiting_ = true;
                                break;
                            }
                        }
                        if (!fx0a_waiting_) {
                            PC_ -= 2; // re-execute this instruction next cycle
                        }
                    } else {
                        // Phase 2: wait for key to be released
                        if (keypad_[fx0a_key_]) {
                            PC_ -= 2; // still held, keep waiting
                        } else {
                            V_[inst_.X]   = fx0a_key_;
                            fx0a_waiting_ = false;
                            fx0a_key_     = 0xFF;
                        }
                    }
                    break;
                }

                case 0x15:
                    // FX15: delay_timer = VX
                    delay_timer_ = V_[inst_.X];
                    break;

                case 0x18:
                    // FX18: sound_timer = VX
                    sound_timer_ = V_[inst_.X];
                    break;

                case 0x1E:
                    // FX1E: I += VX
                    I_ += V_[inst_.X];
                    break;

                case 0x29:
                    // FX29: I = sprite address for digit VX
                    I_ = V_[inst_.X] * 5;
                    break;

                case 0x33: {
                    // FX33: Store BCD of VX at I, I+1, I+2
                    uint8_t bcd  = V_[inst_.X];
                    ram_[I_ + 2] = bcd % 10;
                    bcd /= 10;
                    ram_[I_ + 1] = bcd % 10;
                    bcd /= 10;
                    ram_[I_] = bcd;
                    break;
                }

                case 0x55:
                    // FX55: Dump V0–VX to memory at I
                    for (uint8_t i = 0; i <= inst_.X; ++i) {
                        if (config.current_extension == Extension::CHIP8)
                            ram_[I_++] = V_[i];
                        else
                            ram_[I_ + i] = V_[i];
                    }
                    break;

                case 0x65:
                    // FX65: Load V0–VX from memory at I
                    for (uint8_t i = 0; i <= inst_.X; ++i) {
                        if (config.current_extension == Extension::CHIP8)
                            V_[i] = ram_[I_++];
                        else
                            V_[i] = ram_[I_ + i];
                    }
                    break;

                default:
                    break; // Unimplemented FX opcode
            }
            break;

        default:
            break; // Unimplemented / invalid opcode
    }
}
