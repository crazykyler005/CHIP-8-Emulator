#include <cstdio>
#include <cstring>

#include <chip8.hpp>
#include <random>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <filesystem>
#include "helper_functions.hpp"
#include "byteorder.h"

// implement in different class?
static uint8_t key_pressed() {}

void chip8::draw(uint8_t& x, uint8_t& y, uint8_t pix_height) {}

chip8::chip8() {
	// loading fountset into the designated position in memory (0-80)
	std::copy(std::begin(fontset), std::end(fontset), std::begin(memory));
}

void chip8::reset() {
	program_ctr = program_start_addr;
	opcode = 0;
	index_reg.val = 0;
	stack.clear();
	// stack_ptr = 0;
	memset(registers, 0, sizeof(registers));

	// Clear display
	// reset_display()
	// Clear stack. May not work with uint12_t struct
	memset(stack, 0, sizeof(stack)/sizeof(stack[0]));
	// Clear registers V0-VF
	memset(registers, 0, sizeof(registers));
}

void chip8::initialize() {
	reset();

	// Clear most of the memory
	memset(memory + sizeof(fontset), 0, sizeof(memory) - sizeof(fontset));
}

void chip8::load_program(char* file_name) {
	reset();

	auto file = fopen(file_name, "r");

	if (file == NULL) {
		return;
	}
	
	for (uint16_t addr = program_start_addr; addr < sizeof(memory) && !feof(file); addr++) {
		memory[addr] = fgetc(file);
	}
}

void chip8::run_instruction() {

	switch (opcode & 0xF000)
	{
		uint8_t VX_reg = (opcode >> 8) & 0xF; // 3rd nibble
		uint8_t VY_reg = (opcode >> 4) & 0xF; // 2nd nibble

		case 0x0000:
			if (opcode == 0x00E0) {
				// clear screen
			} else if (opcode == 0x00EE) {
				// program_ctr = stack[stack_ptr];
				program_ctr = stack.pop_back();
				// return from subroutine
			} else if (opcode > 0xFF) {
				printf("Unsupported opcode");
			}
			break;

		case 0x1000: // 1NNN
			// jump to addr at 0NNN
			program_ctr = 0x0FFF & opcode;
			return;

		case 0x2000: // 2NNN
			// call subroutine at 0NNN
			// stack[stack_ptr].val = program_ctr;
			stack.push_back(program_ctr);
			program_ctr = 0x0FFF & opcode;
			break;

		case 0x3000: // 3XNN
			// Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block)
			if (opcode & 0xFF == registers[VX_reg]) {
				program_ctr += 2;
			}
			break;

		case 0x4000: // 4XNN
			// Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
			if (opcode & 0xFF != registers[VX_reg]) {
				program_ctr += 2;
			}

			break;

		case 0x5000: // 5XY0
			// Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
			if (registers[VX_reg] == registers[VY_reg]) {
				program_ctr += 2;
			}

			break;

		case 0x6000: // 6XNN
			// Sets VX to NN
			registers[VX_reg] = (0xFF & opcode);
			break;

		case 0x7000: // 7XNN
			// Adds NN to VX (carry flag is not changed)
			registers[VX_reg] += opcode & 0xFF;
			break;

		case 0x8000: // Sets VX to the value of VY

			// NOTE: 8XY3, 8XY6, 8XY7 and 8XYE were not documented in the original CHIP-8
			// specification so they may never end up being used

			uint8_t code = opcode & 0xF; //arithmetic operation

			if (code == 0) {
				registers[VX_reg] = registers[VY_reg];
			} else if (code == 1) {
				registers[VX_reg] |= registers[VY_reg];
			} else if (code == 2) {
				registers[VX_reg] &= registers[VY_reg];
			} else if (code == 3) {
				registers[VX_reg] ^= registers[VY_reg];
			} else if (code == 4) {
				registers[VX_reg] += registers[VY_reg];
			} else if (code == 5) {
				registers[VX_reg] -= registers[VY_reg];
			} else if (code == 6) {
				registers[VX_reg] >>= 1;
			} else if (code == 7) {
				registers[VX_reg] = registers[VY_reg] - registers[VX_reg];
			} else if (code == 0xE) {
				registers[VX_reg] = 1; 
			}
			break;

		case 0x9000: // 9XY0
			// Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block)
			if (registers[VX_reg] != registers[VY_reg]) {
				program_ctr += 2;
			}
			break;

		case 0xA000:// I = NNN
			// Sets I to the address NNN
			index_reg.val = opcode & 0x0FFF;
			break;

		case 0xB000: // PC = V0 + NNN
			// Jumps to the address NNN plus V0
			program_ctr = registers[0] + (opcode & 0xFFF);
			return;

		case 0xC000: // Vx = rand() & NN
			registers[VX_reg] = static_cast<uint8_t>(rand() & opcode);
			break;

		case 0xD000: // draw(Vx, Vy, N) 
			// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
			// Each row of 8 pixels is read as bit-coded starting from memory location I; 
			// I value does not change after the execution of this instruction. As described above, VF is set
			// to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
			draw(registers[VX_reg], registers[VY_reg], static_cast<uint8_t>(opcode & 0xF));
			break;

		case 0xE000:
			// Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block)
			if (((opcode & 0xFF) == 0x9E) && (key_pressed() == registers[VX_reg])) {
				program_ctr += 2;
			}
			// Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block)
			else if (((opcode & 0xFF) == 0xA1) && (key_pressed() == registers[VX_reg])) {
				program_ctr += 2;
			}
			break;

		case 0xF000:
			uint8_t code = opcode & 0xFF;

			// Sets the delay timer to VX
			if (code == 0x15) {
				// delay_timer(registers[VX_reg]);

			// Sets the sound timer to VX
			} else if (code == 0x18) {
				// sound_timer(registers[VX_reg]);

			// Adds VX to I. VF is not affected
			} else if (code == 0x1E) {
				index_reg.val += registers[VX_reg];

			// Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			} else if (code == 0x29) {
				// index_reg.val = sprite_addr[registers[VX_reg]];

			// Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, 
			// the tens digit at location I+1, and the ones digit at location I+2
			} else if (code == 0x33) {
				uint8_t& VX = registers[VX_reg];
				memory[index_reg.val] = uint8_t(VX / 100);
				memory[index_reg.val + 1] = uint8_t(VX / 10) % 10;
				memory[index_reg.val + 2] = (VX % 100) % 10;

			} else if (code == 0x55) {
				for (uint8_t i = 0; i <= registers[VX_reg]; i++) {
					memory[index_reg.val + i] = registers[i];
				}
			} else if (code == 0x65) {
				for (uint8_t i = 0; i <= registers[VX_reg]; i++) {
					registers[i] = memory[index_reg.val + i];
				}
			}
			break;

		default:
			break;
	}
}


// the program state is saved to a file in the following order
// - 4 byte utc timestamp in seconds
// - 2 byte program counter
// - 2 byte index register
// - 1 byte current stack size
// - all stack elements from first to last added if stack size is not 0 (0 - 32 bytes)
// - the state of each pixel on the screen represented in bits (pixels / 8 = 256)
// - memory from 0x200 and onward (can be overwritten by program thus we need to store it) (4096 – 512 = 3584)
bool chip8::save_program_state(std::string program_name, uint8_t state_number, uint32_t utc_timestamp) {
	// TODO: add a CRC to the end of the file and check if it is valid when loading it

	// 4 + 2 + 2 + 1 + 32 + 256 + 3584
	static size_t MAX_SAVE_FILE_SIZE = 3881

	if (program_name.size() == 0) {
		return false;
	}

	std::vector<uint8_t> buffer(MAX_SAVE_FILE_SIZE);
	uint8_t* idx = buffer.data();

	sys_put_be32(utc_timestamp, idx);
	idx += sizeof(utc_timestamp);

	auto program_ctr_16 = static_cast<uint16_t>(program_ctr.val)
	sys_put_be16(program_ctr_16, idx);
	idx += sizeof(program_ctr_16);

	auto index_reg_16 = static_cast<uint16_t>(program_ctr.val)
	sys_put_be16(index_reg_16, idx);
	idx += sizeof(index_reg_16);

	buffer[idx++] = static_cast<uint8_t>(stack.size());

	for (auto& prog_ctr : stack) {
		auto prog_ctr_16 = static_cast<uint16_t>(prog_ctr)
		sys_put_be16(prog_ctr_16, idx);
		idx += sizeof(prog_ctr_16);
	}

	uint8_t px_byte = 0;
	for (size_t i = 0; i < (sizeof(px_states)/sizeof(px_states[0])); i++) {
		uint8_t px_bit_mask = 1 << (i % 8);

		// if pixel is on
		if (px_states[i]) {
			px_byte |= px_bit_mask;
		}

		if (px_bit_mask & (1 << 7)) {
			buffer[idx++] = px_byte;
			px_byte = 0;
		}
	}

	// get last memory address with relevent data
	uint8_t last_mem_address = 0;
	for (size_t i = 4096 - 1; i >= 0x200; i--) {
		if (memory[i] != 0) {
			last_mem_address = i;
			break;
		}
	}

	for (size_t i = 0x200; i <= last_mem_address; i++) {
		buffer[idx++] = memory[i];
	}

	std::string sav_file_name = program_name + "_" + std::to_string(state_number) + ".sav";

	// will overwrite the file if it already exists
	auto file = fopen(sav_file_name, "w");

	fprintf(file, "%s", buffer.data());
	fclose(file);

	return true;
}

void chip8::load_program_state(std::string file_name) {

	if (file_name.size() == 0) {
		return;
	}

	auto file = fopen(file_name, "r");

	if (file == NULL) {
		return;
	}

	fseek(file, 0L, SEEK_END);
	file_size = ftell(file);

	// seek to beginning of file
	fseek(file, 0L, SEEK_SET);

	std::vector<uint8_t> buffer;
	buffer.reserve(file_size);

	// Don't want to check for EOF char in the case of a false positive
	while (ftell(file) != file_size) {
		buffer.emplace_back(fgetc(file));
	}

	uint8_t* buffer_ptr = buffer.data();
	
	// ignore timestamp
	buffer_ptr += sizeof(uint32_t)

	program_ctr.val = sys_get_be16(buffer_ptr);
	buffer_ptr += sizeof(uint16_t);

	index_reg.val = sys_get_be16(buffer_ptr);
	buffer_ptr += sizeof(uint16_t);

	auto stack_size = buffer_ptr[0];
	buffer_ptr++;

	stack.clear();
	stack.reserve(stack_size);

	for (uint8_t i = 0; i < stack_size; i++) {
		uint12_t ptr_ctr = { sys_get_be16(buffer_ptr) };
		stack.emplace_back(std::move(ptr_ctr));

		buffer_ptr += sizeof(uint16_t);
	}

	px_states[0] = buffer_ptr[0] & 1;
	for (size_t i = 1; i < (sizeof(px_states) / 8); i++) {
		uint8_t px_bit_mask = 1 << (i % 8);
		
		if (px_bit_mask == (1 << 0)) {
			buffer_ptr++;
		}

		px_states[i] = (buffer_ptr[0] & px_bit_mask) ? 1 : 0;
	}

	buffer_ptr++;

	// if the amount of total bytes read from the buffer is less than the file size
	for (uint8_t i = 0; static_cast<uint16_t>(buffer_ptr - buffer.data()) < file_size; i++) {
		memory[0x200 + i] = buffer_ptr[0];
		buffer_ptr++;
	}
}