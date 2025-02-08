#include "../include/chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
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


Chip8::Chip8(const std::string &rom_path) : stack_ptr(stack.data()), rom_name(rom_path) {
  
  load_fontset();
  load_rom(rom_path);

/*
  const uint32_t entry_point = 0x200;       // chip8 roms will be loaded to 0x200
  const uint8_t font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,           // 0
    0x20, 0x60, 0x20, 0x20, 0x70,           // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,           // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,           // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,           // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,           // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,           // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,           // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,           // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,           // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,           // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,           // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,           // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,           // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,           // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,           // F
  };

  // load font
  memcpy(&ram[0], font, sizeof(font));

  // open ROM file
  FILE *rom = fopen(rom_name, "rb");
  if(!rom) {
    SDL_Log("ROM file %s is invalid or does not exist\n", rom_name);
    return;
  }
  
  // get rom size
  fseek(rom, 0, SEEK_END);
  const size_t rom_size = ftell(rom);
  const size_t max_size = sizeof ram - entry_point;
  rewind(rom);

  if(rom_size > max_size) {
    SDL_Log("ROM file %s is too big! Rom size: %zu, Max size allowed: %zu\n",
             rom_name, rom_size, max_size);
    return;
  }
  
  // load ROM
  if(fread(&ram[entry_point], rom_size, 1, rom) != 1){
    SDL_Log("Couldn't read ROM file %s into CHIP8 memory\n", rom_name);
    return;
  }


  fclose(rom);

  // set chip8 machine defaults
  state = EmulatorState::RUNNING;           // default machine state
  PC = entry_point;                         // start program counter at ROM entry point
  Chip8::rom_name = rom_name;
  stack_ptr = &stack[0];
*/
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

void Chip8::handle_input() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        // exit window; end program
        state = QUIT;
        return ;
      break;

      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
          case SDLK_ESCAPE:
            // escape key; exit window & end program
            state = QUIT;
            return;
          break;

          case SDLK_SPACE:
            // space bar
            if(state == RUNNING) {
              state = PAUSED;                       // pause
              std::cout << "========= PAUSED =========" << "\n";
            }
            else {
              state = RUNNING;                      // resume
            }
            return;
          break;
        }
      break;

      case SDL_KEYUP:

      break;
    }
  }
}
/*
EmulatorState Chip8::get_state() {
  return state;
}
*/
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

    case 0x0A:
      // 0xANNN: Set index register I to NNN
      printf("Set I to NNN (0x%04X)\n",
              inst.NNN);
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
        memset(&display[0], 0, sizeof display);
      }
      else if(inst.NN == 0xEE) {
        // 0x00EE: Return from a subroutine
        // set program counter to last address on subroutine stack
        PC = *--stack_ptr;
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

    case 0x06:
      // 0x6XNN: Set register VX to NN
      V[inst.X] = inst.NN;
    break;
    
    case 0x07:
      // 0x7XNN: Set register VX += NN
      V[inst.X] += inst.NN;  
    break;

    case 0x0A:
      // 0xANNN: Set index register I to NNN
      I = inst.NNN;
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

    break;
    }

    default:
      break;    // Unimplemented or invalid opcode
  }
}
