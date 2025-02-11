#include "../include/chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>

constexpr std::array<uint8_t, 80> FONTSET = {
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
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};


Chip8::Chip8(const std::string &rom_path, Audio *audio_ref) : 
  stack_ptr(stack.data()),
  rom_name(rom_path),
  audio(audio_ref)
{
  load_fontset();
  load_rom(rom_path);
}

void Chip8::load_fontset() {
  std::copy(FONTSET.begin(), FONTSET.end(), ram.begin());
}

void Chip8::load_rom(const std::string& rom_path) {
  std::ifstream rom(rom_path, std::ios::binary | std::ios::ate);
  if(!rom) {
    std::cerr << "Error: ROM file " << rom_path << " is invalid or does not exist.\n";
    state = EmulatorState::QUIT;
    return;
  }

  std::streamsize romSize = rom.tellg();
  const size_t maxSize = RAM_SIZE - ROM_START;
  if(static_cast<size_t>(romSize) > maxSize) {
    std::cerr << "Error: ROM file " << rom_path << " is too big! Max size allowed: " << maxSize << " bytes.\n";
    state = EmulatorState::QUIT;
    return;
  }

  rom.seekg(0, std::ios::beg);
  rom.read(reinterpret_cast<char*>(&ram[ROM_START]), romSize);
}

void Chip8::run() {
  // CHIP-8 execution loop (To be implemented later)
}

void Chip8::handle_input(Config &config) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        // exit window; end program
        state = QUIT;
      break;

      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
          case SDLK_ESCAPE:
            // escape key; exit window & end program
            state = QUIT;
          break;

          case SDLK_SPACE:
            // space bar
            if(state == RUNNING) {
              state = PAUSED;                       // pause
              std::cout << "========= PAUSED =========" << "\n";
            }
            else {
              state = RUNNING;                      // resume
              std::cout << "========= RUNNING =========" << "\n";
            }
            return;
          break;

          case SDLK_EQUALS:
            // '=': reset CHIP8 machine for current ROM
            reset();
          break;

          case SDLK_j:
            // 'j': Decrease color lerp rate
            if(config.color_lerp_rate > 0.1)
              config.color_lerp_rate -= 0.1;
          break;
          
          case SDLK_k:
            // 'k': Increase color lerp rate
            if(config.color_lerp_rate < 1.0)
              config.color_lerp_rate += 0.1;
          break;
          
          case SDLK_o:
            // 'o': Decrease volume
            if(config.volume > 0)
              config.volume -= 500;
          break;
          
          case SDLK_p:
            // 'p': Increase volume
            if(config.volume < INT16_MAX)
              config.volume += 500;
          break;
          
          // Map qwerty keys to CHIP8 keypad
          case SDLK_1: keypad[0x1] = true; break;
          case SDLK_2: keypad[0x2] = true; break;
          case SDLK_3: keypad[0x3] = true; break;
          case SDLK_4: keypad[0xC] = true; break;

          case SDLK_q: keypad[0x4] = true; break;
          case SDLK_w: keypad[0x5] = true; break;
          case SDLK_e: keypad[0x6] = true; break;
          case SDLK_r: keypad[0xD] = true; break;
          
          case SDLK_a: keypad[0x7] = true; break;
          case SDLK_s: keypad[0x8] = true; break;
          case SDLK_d: keypad[0x9] = true; break;
          case SDLK_f: keypad[0xE] = true; break;
          
          case SDLK_z: keypad[0xA] = true; break;
          case SDLK_x: keypad[0x0] = true; break;
          case SDLK_c: keypad[0xB] = true; break;
          case SDLK_v: keypad[0xF] = true; break;

          default:
            break;
        }
      break;

      case SDL_KEYUP:
        switch(event.key.keysym.sym) {
          // Map qwerty keys to CHIP8 keypad
          case SDLK_1: keypad[0x1] = false; break;
          case SDLK_2: keypad[0x2] = false; break;
          case SDLK_3: keypad[0x3] = false; break;
          case SDLK_4: keypad[0xC] = false; break;

          case SDLK_q: keypad[0x4] = false; break;
          case SDLK_w: keypad[0x5] = false; break;
          case SDLK_e: keypad[0x6] = false; break;
          case SDLK_r: keypad[0xD] = false; break;
          
          case SDLK_a: keypad[0x7] = false; break;
          case SDLK_s: keypad[0x8] = false; break;
          case SDLK_d: keypad[0x9] = false; break;
          case SDLK_f: keypad[0xE] = false; break;
          
          case SDLK_z: keypad[0xA] = false; break;
          case SDLK_x: keypad[0x0] = false; break;
          case SDLK_c: keypad[0xB] = false; break;
          case SDLK_v: keypad[0xF] = false; break;

          default:
            break;
        }
      break;

      default:
        break;
    }
  }
}
#ifdef DEBUG
void Chip8::print_debug_info() const {
  printf("Address: 0x%04X, Opcode: 0x%04X, Desc: ",
          PC-2, inst.opcode);
  
  switch((inst.opcode >> 12) & 0x0F) {
    case 0x00:
      if(inst.NN == 0xE0) {
        // 0x00E0: Clear the screen
        printf("Clear screen\n");
      }
      else if(inst.NN == 0xEE) {
        // 0x00EE: Return from a subroutine
        // set program counter to last address on subroutine stack
        printf("Return from subroutine to address 0x%04X\n",
                *(stack_ptr-1));
      }
    break;
    
    case 0x01:
      // 0x1NNN: Jump to address NNN
      printf("Jump to address NNN (0x%04X)\n",
              inst.NNN);
    break;

    case 0x02:
      // 0x2NNN: Call subroutine at NNN
      printf("Call subroutine at NNN (0x%04X)\n",
              inst.NNN);
    break;
    
    case 0x03:
      // 0x3XNN: check if VX == NN, if so, skip the next instruction.
      printf("check if v%x (0x%02x) == nn (0x%02x), skip next instruction if true\n",
             inst.X, V[inst.X], inst.NN);
    break;
    
    case 0x04:
      // 0x4XNN: check if VX != NN, if so, skip the next instruction.
      printf("Check if V%X (0x%02X) != NN (0x%02X), skip next instruction if true\n",
             inst.X, V[inst.X], inst.NN);
    break;
    
    case 0x05:
      // 0x5XY0: Check if VX == VY, if so, skip the next instruction
      printf("check if v%x (0x%02x) == V%x (0x%02x), skip next instruction if true\n",
             inst.X, V[inst.Y], inst.Y, V[inst.Y]);
    break;
    
    case 0x06:
      // 0x6XNN: Set register VX to NN
      printf("Set register V%X to NN (0x%02X)\n",
              inst.X, inst.NN);
      break;

    case 0x07:
      // 0x7XNN: Set register VX += NN
      printf("Set register V%X (0x%02X) += NN (0x%02X). Result: 0x%02X\n",
              inst.X, V[inst.X], inst.NN,
              V[inst.X] + inst.NN);
    break;
    
    case 0x08:
      switch(inst.N) {
        case 0:
          // 0x8XY0: Set register VX = VY
          printf("Set register V%X = V%X (0x%02X)\n",
                  inst.X, inst.Y, V[inst.Y]);
        break;

        case 1:
          // 0x8XY1: Set register VX |= VY
      printf("Set register V%X (0x%02X) |= V%X (0x%02X). Result: 0x%02X\n",
              inst.X, V[inst.X],
              inst.Y, V[inst.Y],
              V[inst.X] | V[inst.Y]);
        break;
        
        case 2:
          // 0x8XY2: Set register VX &= VY
      printf("Set register V%X (0x%02X) &= V%X (0x%02X). Result: 0x%02X\n",
              inst.X, V[inst.X],
              inst.Y, V[inst.Y],
              V[inst.X] & V[inst.Y]);
        break;

        case 3:
          // 0x8XY3: Set register VX ^= VY
      printf("Set register V%X (0x%02X) ^= V%X (0x%02X). Result: 0x%02X\n",
              inst.X, V[inst.X],
              inst.Y, V[inst.Y],
              V[inst.X] ^ V[inst.Y]);
        break;

        case 4:
          // 0x8XY4: Set register VX += VY, set VF to 1 if carry
      printf("Set register V%X (0x%02X) += V%X (0x%02X). VF = 1 if carry, Result: 0x%02X, VF = %X\n",
              inst.X, V[inst.X],
              inst.Y, V[inst.Y],
              V[inst.X] + V[inst.Y],
              ((uint16_t)(V[inst.X] + V[inst.Y]) > 255));
        break;

        case 5:
          // 0x8XY5: Set register VX -= VY, Set VF to 1 if ther is not a borrow
      printf("Set register V%X (0x%02X) -= V%X (0x%02X). VF = 1 if no borrow, Result: 0x%02X, VF = %X\n",
              inst.X, V[inst.X],
              inst.Y, V[inst.Y],
              V[inst.X] - V[inst.Y],
              (V[inst.Y] <= V[inst.X]));
        break;

        case 6:
          // 0x8XY6: Set register VX >>= 1, store shifted off bit in VF.
      printf("Set register V%X (0x%02X) >>= 1. VF = 1 shifted off bit (%X), Result: 0x%02X\n",
              inst.X, V[inst.X],
              V[inst.X] & 1,
              V[inst.X] >> 1);
        break;

        case 7:
          // 0x8XY7: Set register VX = VY - VX, Set VF to 1 if ther is not a borrow
      printf("Set register V%X = V%X (0x%02X) - V%X (0x%02X). VF = 1 if no borrow, Result: 0x%02X, VF = %X\n",
              inst.X,
              inst.Y, V[inst.Y],
              inst.X, V[inst.X],
              V[inst.Y] - V[inst.X],
              (V[inst.X] <= V[inst.Y]));
        break;

        case 0xE:
          // 0x8XY6: Set register VX <<= 1, store shifted off bit in VF.
      printf("Set register V%X (0x%02X) <<= 1. VF = 1 shifted off bit (%X), Result: 0x%02X\n",
              inst.X, V[inst.X],
              (V[inst.X] & 0x80) >> 7,
              V[inst.X] << 1);
        break;
      }
    break;
    
    case 0x09:
      // 0x9XY0: Check if VX != VY, skip next instruction if so
      printf("check if v%x (0x%02x) != V%x (0x%02x), skip next instruction if true\n",
             inst.X, V[inst.Y], inst.Y, V[inst.Y]);
    break;

    case 0x0A:
      // 0xANNN: Set index register I to NNN
      printf("Set I to NNN (0x%04X)\n",
              inst.NNN);
    break;
    
    case 0x0B:
      // 0xBNNN: Jump to address NNN + V0
      printf("Set PC to NNN (0x%04X) + V0 (0x%02X). Result: PC = 0x%04X\n",
              inst.NNN, V[0], inst.NNN + V[0]);
    break;
    
    case 0x0C:
      // 0xCXNN: Sets register VX = rand() % 256 & NN
      printf("Set V%X = rand() %% 256 & NN (0x%02X)\n",
              V[inst.X], inst.NN);
    break;

    case 0x0D:
      // 0xDXYN: Draw N-height sprite at coords X,Y; Read from memory location I;
      // Screen pixels are XOR'd with sprite bits,
      // VF (carry flag) is set if any screen pixels are set off;
      // this is useful for collision detection.
      printf("Draw N (%u) height sprite at coords V%X (0x%02X), V%X (0x%02X) "
             "from memory location I (0x%04X). Set VF = 1 if any pixels are turned off.\n",
             inst.N, inst.X, V[inst.X], inst.Y,
             V[inst.Y], I);
    break;
    
    case 0x0E:
      if(inst.NN == 0x9E) {
        // 0xEX9E: Skip next instruction if key in VX is pressed
        printf("Skip next instruction if key in V%X (0x%02X) is pressed. Keypad value: %d\n",
                inst.X, V[inst.X],
                keypad[V[inst.X]]);
      }
      else if(inst.NN == 0xA1) {
        // 0xEXA1: Skip next instruction if key in VX is not pressed
        printf("Skip next instruction if key in V%X (0x%02X) is not pressed. Keypad value: %d\n",
                inst.X, V[inst.X],
                keypad[V[inst.X]]);
      }
    break;
    
    case 0x0F:
      switch(inst.NN) {
        case 0x0A:
          // 0xFX0A: get_key(); Await until a keypress, and store it in VX
          printf("Await until a key is pressed; store key in V%X\n",
                  inst.X);
        break;
        
        case 0x1E:
          // 0xFX1E: I += VX; Add VX to register I
          printf("I (0x%04X) += V%X (0x%02X). Result (I): 0x%04X\n",
                  I, inst.X, V[inst.X],
                  I + V[inst.X]);
        break;
        
        case 0x07:
          // 0xFX07: VX = delay timer
          printf("Set V%X = delay timer value (0x%02X)\n",
                  inst.X, delay_timer); 
        break;
        
        case 0x15:
          // 0xFX15: delay timer = VX
          printf("Set delay timer value = V%X (0x%02X)\n",
                  inst.X, V[inst.X]); 
        break;
        
        case 0x18:
          // 0xFX18: sound timer = VX
          printf("Set sound timer value = V%X (0x%02X)\n",
                  inst.X, V[inst.X]); 
        break;
        
        case 0x29:
          // 0xFX29: Set register I to sprite location in memory for character in VX (0x0 - 0xF)
          printf("Set I to sprite location in memory for character in V%X (0x%02X). Result (V%X * 5) = (0x%02X)\n",
                  inst.X, V[inst.X], inst.X, V[inst.X] * 5);
        break;
        
        case 0x33:
          // 0xFX33: Store BCD representation of VX at memory offset from I
          // I = hundred's place, I+1 = ten's place, I+2 = one's place
          printf("Store BCD representation V%X (0x%02X) at memory I (0x%04X)\n",
                  inst.X, V[inst.X], I);
        break;
        
        case 0x55:
          // 0xFX55: Register dump V0 - VX inclusive to memory offset from I
          printf("Register dump V0 - V%X (0x%02X) inclusive at memory from I (0x%04X)\n",
                  inst.X, V[inst.X], I);
        break;
        
        case 0x65:
          // 0xFX65: Register load V0 - VX inclusive to memory offset from I
          printf("Register load V0 - V%X (0x%02X) inclusive at memory from I (0x%04X)\n",
                  inst.X, V[inst.X], I);
        break;

        default:
          break;  // Unimplemented
      }
    break;

    default:
      printf("Unimplemented opcode\n");
    break;
  }
}
#endif // DEBUG


void Chip8::emulate_instruction(const Config &config) {
  // Get next opcode from ram
  inst.opcode = (ram[PC] << 8) | ram[PC+1];
  
  // Pre-increment program counter for next opcode
  PC += 2;  
  
  // Fill out current instruction format
  inst.NNN = inst.opcode & 0x0FFF;
  inst.NN = inst.opcode & 0x0FF;
  inst.N = inst.opcode & 0x0F;
  inst.X = (inst.opcode >> 8) & 0x0F;
  inst.Y = (inst.opcode >> 4) & 0x0F;

#ifdef DEBUG
  Chip8::print_debug_info(); 
#endif // DEBUG

  // Emulate opcode
  switch((inst.opcode >> 12) & 0x0F) {
    case 0x00:
      if(inst.NN == 0xE0) {
        // 0x00E0: Clear the screen
        std::fill(display.begin(), display.end(), false);
        set_draw_flag(true);
      }
      else if(inst.NN == 0xEE) {
        // 0x00EE: Return from a subroutine
        // set program counter to last address on subroutine stack
        PC = *--stack_ptr;
      }
      else {
        // Unimplemented / invalid opcode
        // maybe 0xNNN for calling machine code routine for RCA1802
      }
    break;
    
    case 0x01:
      // 0x1NNN: Jump to address NNN
      PC = inst.NNN;
    break;

    case 0x02:
      // 0x2NNN: Call subroutine at NNN
      *stack_ptr++ = PC;    // store current address to return to on subroutine stack
      PC = inst.NNN;      // set program counter to subroutine address
    break;

    case 0x03:
      // 0x3XNN: check if VX == NN, if so, skip the next instruction.
      if(V[inst.X] == inst.NN)
        PC += 2;          // skip next opcode / instruction
    break;
    
    case 0x04:
      // 0x4XNN: check if VX != NN, if so, skip the next instruction.
      if(V[inst.X] != inst.NN)
        PC += 2;          // skip next opcode / instruction
    break;

    case 0x05:
      // 0x5XY0: Check if VX == VY, if so, skip the next instruction
      if(inst.N != 0) break;     // wrong opcoed (not implemented)
      if(V[inst.X] == V[inst.Y])
        PC += 2;          // skip next opcode / instruction
    break;

    case 0x06:
      // 0x6XNN: Set register VX to NN
      V[inst.X] = inst.NN;
    break;
    
    case 0x07:
      // 0x7XNN: Set register VX += NN
      V[inst.X] += inst.NN;  
    break;

    case 0x08:
      switch(inst.N) {
        case 0:
          // 0x8XY0: Set register VX = VY
          V[inst.X] = V[inst.Y];
        break;

        case 1:
          // 0x8XY1: Set register VX |= VY
          V[inst.X] |= V[inst.Y];
        break;
        
        case 2:
          // 0x8XY2: Set register VX &= VY
          V[inst.X] &= V[inst.Y];
        break;

        case 3:
          // 0x8XY3: Set register VX ^= VY
          V[inst.X] ^= V[inst.Y];
        break;

        case 4:
          // 0x8XY4: Set register VX += VY, set VF to 1 if carry
          V[0xF] = ((uint16_t)(V[inst.X] + V[inst.Y]) > 255);

          V[inst.X] += V[inst.Y];
        break;

        case 5:
          // 0x8XY5: Set register VX -= VY, Set VF to 1 if ther is not a borrow
          V[0xF] = (V[inst.Y] <= V[inst.X]);
          V[inst.X] -= V[inst.Y];
        break;

        case 6:
          // 0x8XY6: Set register VX >>= 1, store shifted off bit in VF.
          V[0xF] = V[inst.X] & 1;
          V[inst.X] >>= 1;
        break;

        case 7:
          // 0x8XY7: Set register VX = VY - VX, Set VF to 1 if ther is not a borrow
          V[0xF] = (V[inst.X] <= V[inst.Y]);
          V[inst.X] = V[inst.Y] - V[inst.X];
        break;

        case 0xE:
          // 0x8XY6: Set register VX <<= 1, store shifted off bit in VF.
          V[0xF] = (V[inst.X] & 0x80) >> 7;
          V[inst.X] <<= 1;
        break;
      }
    break;

    case 0x09:
      // 0x9XY0: Check if VX != VY, skip next instruction if so
      if(V[inst.X] != V[inst.Y])
        PC += 2;
    break;

    case 0x0A:
      // 0xANNN: Set index register I to NNN
      I = inst.NNN;
    break;

    case 0x0B:
      // 0xBNNN: Jump to address NNN + V0
      PC = inst.NNN + V[0];
    break;

    case 0x0C:
      // 0xCXNN: Sets register VX = rand() % 256 & NN
      V[inst.X] = (rand() % 256) & inst.NN;
    break;

    case 0x0D: {
      // 0xDXYN: Draw N-height sprite at coords X,Y; Read from memory location I;
      // Screen pixels are XOR'd with sprite bits,
      // VF (carry flag) is set if any screen pixels are set off;
      // this is useful for collision detection.
      uint8_t X_coord = V[inst.X] % config.window_width;
      uint8_t Y_coord = V[inst.Y] % config.window_height;
      const uint8_t orig_X = X_coord;         // original X value

      V[0xF] = 0;       // initialize carry flag to 0
      
      // loop over all N rows of sprite
      for(uint8_t i = 0; i < inst.N; i++) {
        // get next byte/row of sprite data
        const uint8_t sprite_data = ram[I + i];
        
        X_coord = orig_X;     // reset X for next raw to draw 
        for(int8_t j = 7; j >= 0; j--) {
          // if sprite pixel/bit is on and display pixel is on, set carry flag
          bool *pixel = &display[Y_coord * config.window_width + X_coord];
          const bool sprite_bit = (sprite_data & (1 << j));
          
          if(sprite_bit && *pixel) {
            V[0xF] = 1;
          }

          // XOR display pixel with sprite pixel/data to set it on or off
          *pixel ^= sprite_bit;

          // Stop drawing this row if hit right edge of screen
          if(++X_coord >= config.window_width)
            break;
        }
        // Stop drawing entire sprite if hit bottom edge of screen
        if(++Y_coord >= config.window_height) 
          break;
      }
    
    set_draw_flag(true);
    break;
    }

    case 0x0E:
      if(inst.NN == 0x9E) {
        // 0xEX9E: Skip next instruction if key in VX is pressed
        if(keypad[V[inst.X]])
          PC += 2;
      }
      else if(inst.NN == 0xA1) {
        // 0xEXA1: Skip next instruction if key in VX is not pressed
        if(!keypad[V[inst.X]])
          PC += 2;
      }
    break;

    case 0x0F:
      switch(inst.NN) {
        case 0x0A: {
          // 0xFX0A: get_key(); Await until a keypress, and store it in VX
          bool any_key_pressed = false;
          for(uint8_t i = 0; i < sizeof keypad; i++) {
            if(keypad[i]) {
              V[inst.X] = i;
              any_key_pressed = true;
              break;
            }
          }
          if(!any_key_pressed) {
            // wait without running other instruction
            // (keep getting current opcode and running this instruction)
            PC -= 2;
          }
        break;
        }
        
        case 0x1E:
          // 0xFX1E: I += VX; Add VX to register I
          I += V[inst.X];
        break;
        
        case 0x07:
          // 0xFX07: VX = delay timer
          V[inst.X] = delay_timer;
        break;
        
        case 0x15:
          // 0xFX15: delay timer = VX
          delay_timer = V[inst.X];
        break;
        
        case 0x18:
          // 0xFX18: sound timer = VX
          sound_timer = V[inst.X];
        break;
        
        case 0x29:
          // 0xFX29: Set register I to sprite location in memory for character in VX (0x0 - 0xF)
          I = V[inst.X] * 5;
        break;
        
        case 0x33: {
          // 0xFX33: Store BCD representation of VX at memory offset from I
          // I = hundred's place, I+1 = ten's place, I+2 = one's place
          uint8_t bcd = V[inst.X];
          ram[I+2] = bcd % 10;
          bcd /= 10;
          ram[I+1] = bcd % 10;
          bcd /= 10;
          ram[I] = bcd;
        break;
        }
        
        case 0x55:
          // 0xFX55: Register dump V0 - VX inclusive to memory offset from I
          for(uint8_t i = 0; i <= inst.X; i++) {
            ram[I + i] = V[i];
          }
        break;
        
        case 0x65:
          // 0xFX65: Register load V0 - VX inclusive to memory offset from I
          for(uint8_t i = 0; i <= inst.X; i++) {
            V[i] = ram[I + i];
          }
        break;

        default:
          break;  // Unimplemented
      }
    break;

    default:
    break;    // Unimplemented or invalid opcode
  }
}

// Update CHIP8 delay and sound timers
void Chip8::update_timers() {
  if(delay_timer > 0) {
    delay_timer--;
  }
  if(sound_timer > 0) {
    sound_timer--;
    audio->play();    // play sound
  }
  else {
    audio->stop();    // pause sound
  }
}

void Chip8::reset() {
  // Reset RAM, display, and stack using std::fill
  ram.fill(0);
  display.fill(false);
  stack.fill(0);
  keypad.fill(false);
  V.fill(0);

  // Reset CPU registers and state
  I = 0;
  PC = ROM_START;
  stack_ptr = &stack[0]; // Set stack_ptr to point to the start of stack
  delay_timer = 0;
  sound_timer = 0;
  draw = true;  // Mark screen for redraw

  // Reload ROM if it was previously loaded
  if(!rom_name.empty()) {
    load_rom(rom_name);
  }

  std::cout << "========= CHIP-8 RESET =========" << std::endl;
}
