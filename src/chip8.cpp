
#include "chip8.hpp"
#include "helper_functions.hpp"
#include "byteorder.h"

#include <random>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <filesystem>
#include <cstring>

Chip8::Chip8() {
	// loading fontset into the designated position in memory (0-80)
	std::copy(std::begin(fontset), std::end(fontset), std::begin(memory));
}

void Chip8::update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height) {
	// Reset register VF
	registers[0xF] = 0;
	// printf("x: %d, y: %d\n", x, y);


	for (uint8_t yline = 0; yline < pix_height; yline++)
	{
		// Fetch the pixel value from the memory starting at location I
		uint8_t _8px_sprite = memory[index_reg + yline];

		// Loop over 8 bits of one row
		for (int xline = 0; xline < SPRITE_PX_WIDTH; xline++) {
			// Check if the current evaluated pixel is set to 1 (note that 0x80 >> xline scan through the byte, one bit at the time)
			if ((_8px_sprite & (0x80 >> xline)) != 0) {
				// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
				if (px_states[(x + xline + ((y + yline) * native_width))] == 1) {
					registers[0xF] = 1;
				}

				// Set the pixel value by using XOR
				px_states[x + xline + ((y + yline) * native_width)] ^= 1;
			}
		}
	}

	draw_flag = true;
	
	return;
}

void Chip8::reset() {
	program_ctr = program_start_addr;
	index_reg = 0;

	// clear stack
	stack.clear();

	// clear all registers
	memset(registers, 0, sizeof(registers));

	// Clear display
	memset(px_states, 0, sizeof(px_states));
	draw_flag = true;

	// Clear registers V0-VF
	memset(registers, 0, sizeof(registers));

	// Clears key_presses
	memset(keys_pressed, false, sizeof(keys_pressed));

	is_paused = false;
	play_sfx = false;
	lowest_mem_addr_updated = 0xFFF;
}

bool Chip8::load_program(std::string file_path) {
	reset();

	auto file = fopen((file_path).c_str(), "r");

	if (file == NULL) {
		return false;
	}
	
	memset(memory + sizeof(fontset), 0, sizeof(memory) - sizeof(fontset));

	for (uint16_t addr = program_start_addr; addr < sizeof(memory) && !feof(file); addr++) {
		memory[addr] = fgetc(file);
	}

	return true;
}

void Chip8::run_instruction() {

	uint16_t opcode = (static_cast<uint16_t>(memory[program_ctr]) << 8) + memory[program_ctr + 1];

	uint8_t VX_reg = (opcode >> 8) & 0xF; // 3rd nibble
	uint8_t VY_reg = (opcode >> 4) & 0xF; // 2nd nibble

	// printf("opcode: %x, i: %d, pc: %d, reg[vx]: %d, VX_reg: %d\n", opcode, index_reg, program_ctr, registers[VX_reg], VX_reg);

	uint8_t sub_opcode = 0;

	switch (opcode & 0xF000)
	{
		case 0x0000:
			if (opcode == 0x00E0) {
				// clear screen
				memset(px_states, 0, sizeof(px_states));
				draw_flag = true;

			} else if (opcode == 0x00EE) {
				if (stack.size() == 0) {
					printf("Cannot return from subroutine\n");
					break;
				}

				// return from subroutine
				program_ctr = stack.back();
				stack.pop_back();

			} else if (opcode > 0xFF) {
				printf("Unsupported opcode\n");
				// chip8.is_paused = true;
			}
			break;

		case 0x1000: // 1NNN
			// jump to addr at 0NNN
			program_ctr = opcode & 0x0FFF;
			return;

		case 0x2000: // 2NNN
			// call subroutine at 0NNN
			// stack[stack_ptr] = program_ctr;
			stack.push_back(program_ctr);
			program_ctr =  opcode & 0x0FFF;
			return;

		case 0x3000: // 3XNN
			// Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block)
			if ((opcode & 0xFF) == registers[VX_reg]) {
				program_ctr += 2;
			}

			break;

		case 0x4000: // 4XNN
			// Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block)
			if ((opcode & 0xFF) != registers[VX_reg]) {
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
			registers[VX_reg] = (opcode & 0xFF);
			break;

		case 0x7000: // 7XNN
			// Adds NN to VX (carry flag is not changed)
			registers[VX_reg] += (opcode & 0xFF);
			break;

		case 0x8000: // Sets VX to the value of VY

			// NOTE: 8XY3, 8XY6, 8XY7 and 8XYE were not documented in the original CHIP-8
			// specification so they may never end up being used

			sub_opcode = (opcode & 0xF); //arithmetic operation

			if (sub_opcode == 0) {
				registers[VX_reg] = registers[VY_reg];
			} else if (sub_opcode == 1) {
				registers[VX_reg] |= registers[VY_reg];
			} else if (sub_opcode == 2) {
				registers[VX_reg] &= registers[VY_reg];
			} else if (sub_opcode == 3) {
				registers[VX_reg] ^= registers[VY_reg];

			// Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
			} else if (sub_opcode == 4) {
				registers[VX_reg] += registers[VY_reg];
				registers[0xF] = (registers[VY_reg] > (0xFF - registers[VX_reg])) ? 1 : 0;

			// VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not.
			} else if (sub_opcode == 5) {
				registers[0xF] = (registers[VY_reg] > registers[VX_reg]) ? 0 : 1;
				registers[VX_reg] -= registers[VY_reg];

			// Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
			} else if (sub_opcode == 6) {
				registers[0xF] = registers[VX_reg] & 0x1;
				registers[VX_reg] >>= 1;

			// Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not
			} else if (sub_opcode == 7) {
				registers[0xF] = (registers[VX_reg] > registers[VY_reg]) ? 0 : 1;
				registers[VX_reg] = registers[VY_reg] - registers[VX_reg];

			// Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
			} else if (sub_opcode == 0xE) {
				registers[0xF] = (registers[VX_reg] & 0x80) ? 1 : 0;
				registers[VX_reg] <<= 1;
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
			index_reg = opcode & 0x0FFF;
			break;

		case 0xB000: // PC = V0 + NNN
			// Jumps to the address NNN plus V0
			program_ctr = registers[0] + (opcode & 0xFFF);
			return;

		case 0xC000: // Vx = rand() & NN
			registers[VX_reg] = static_cast<uint8_t>(rand() & (opcode & 0xFF));
			break;

		case 0xD000: // draw(Vx, Vy, N) 
			// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
			// Each row of 8 pixels is read as bit-coded starting from memory location I; 
			// I value does not change after the execution of this instruction. As described above, VF is set
			// to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
			update_gfx(registers[VX_reg], registers[VY_reg], static_cast<uint8_t>(opcode & 0xF));
			break;

		case 0xE000:
			// Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block)
			if ((opcode & 0xFF) == 0x9E) {
				if (keys_pressed[registers[VX_reg]] == true) {
					program_ctr += 2;
				}
			}
			// Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block)
			else if ((opcode & 0xFF) == 0xA1) {
				if (keys_pressed[registers[VX_reg]] == false) {
					program_ctr += 2;
				}

			} else {
				printf("\nUnknown op code: %.4X\n", opcode);
			}
			break;

		case 0xF000:
			sub_opcode = opcode & 0xFF;

			// Sets VX to the value of the delay timer.[24]
			if (sub_opcode == 0x07) {
				registers[VX_reg] = delay_timer;
			}

			// A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event)
			if (sub_opcode == 0x0A) {
				bool key_pressed = false;

				for(uint8_t i = 0; i < 16; i++)
				{
					if (keys_pressed[i])
					{
						registers[VX_reg] = i;
						key_pressed = true;
					}
				}

				if (key_pressed) {
					return;
				}
			}

			// Sets the delay timer to VX
			if (sub_opcode == 0x15) {
				delay_timer = registers[VX_reg];

			// Sets the sound timer to VX
			} else if (sub_opcode == 0x18) {
				sound_timer = registers[VX_reg];

			// Adds VX to I. The CHIP-8 interpreter for the Commodore Amiga sets VF to 1 when an overflow occurs from this.
			// There is one known game that depends on this happening and at least one that doesn't.
			} else if (sub_opcode == 0x1E) {
				index_reg += registers[VX_reg];

				// TODO: implement a setting from the menubar that enables this functionallity
				if (_0xFX1E_overflow_enabled) {
					registers[0xF] = (index_reg & 0xF000) ? 1 : 0;
				}

			// Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			} else if (sub_opcode == 0x29) {
				index_reg = registers[VX_reg] * 5;

			// Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, 
			// the tens digit at location I+1, and the ones digit at location I+2
			} else if (sub_opcode == 0x33) {
				uint8_t& VX = registers[VX_reg];
				memory[index_reg] = uint8_t(VX / 100);
				memory[index_reg + 1] = uint8_t(VX / 10) % 10;
				memory[index_reg + 2] = (VX % 100) % 10;

				if (index_reg < lowest_mem_addr_updated) {
					lowest_mem_addr_updated = index_reg;
				}

			// Stores from V0 to VX (including VX) in memory, starting at address I. The offset
			// from I is increased by 1 for each value written, but I itself is left unmodified.

			// Before the CHIP-8 interpreters CHIP48 and SUPER-CHIP (1970s - 1980s), the I register
			// was incremented each time it stored or loaded one register. (I += X + 1).
			} else if (sub_opcode == 0x55) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					memory[index_reg + i] = registers[i];
				}

				if (index_reg < lowest_mem_addr_updated) {
					lowest_mem_addr_updated = index_reg;
				}

				index_reg += VX_reg + 1;

			// Fills from V0 to VX (including VX) with values from memory, starting at address I.
			// The offset from I is increased by 1 for each value read, but I itself is left unmodified.
			} else if (sub_opcode == 0x65) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					registers[i] = memory[index_reg + i];
				}

				index_reg += VX_reg + 1;
			}
			break;

		default:
			break;
	}

	program_ctr += 2;
}

void Chip8::countdown_timers() 
{
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
		if(sound_timer == 1);
			play_sfx = false;

		// TODO: Implement sound
		--sound_timer;
}

void Chip8::run() 
{
	run_instruction();
	// always want to count down the timers even if the program counter is not updated
	countdown_timers();
}


// the program state is saved to a file in the following order
// - 4 byte utc timestamp in seconds
// - 2 byte program counter
// - 2 byte index register
// - 1 byte current stack size
// - all stack elements from first to last added if stack size is not 0 (0 - 32 bytes)
// - 256 bytes that hold the state of 8 pixels in each byte (pixels / 8 = 256)
// - 2 byte lowest memory address update by the program/game
// - x byte memory from lowest memory address overwritten and onward
// - TODO: add a CRC to the end of the file and check if it is valid when loading it
bool Chip8::save_program_state(uint8_t state_number, uint32_t utc_timestamp) 
{
	return false;

	if (program_name.size() == 0) {
		return false;
	}

	// 4 + 2 + 2 + 1 + 256 (everything above excluding stack elements, CRC size)
	static size_t MIN_SAVE_FILE_SIZE = 265;
	size_t save_file_size = MIN_SAVE_FILE_SIZE + (stack.size() * sizeof(uint16_t));

	// get last memory address with relevent data
	uint8_t last_mem_addr = 0;
	if (lowest_mem_addr_updated != (MEMORY_SIZE - 1)) {
		for (size_t i = 4096 - 1; i >= 0x200; i--) {
			if (memory[i] != 0) {
				last_mem_addr = i;
				break;
			}
		}

		save_file_size += sizeof(uint16_t) + (last_mem_addr - lowest_mem_addr_updated + 1);
	}

	std::vector<uint8_t> buffer(save_file_size);
	uint8_t* idx = buffer.data();

	sys_put_be32(utc_timestamp, idx);
	idx += sizeof(utc_timestamp);

	auto program_ctr_16 = static_cast<uint16_t>(program_ctr);
	sys_put_be16(program_ctr_16, idx);
	idx += sizeof(program_ctr_16);

	auto index_reg_16 = static_cast<uint16_t>(program_ctr);
	sys_put_be16(index_reg_16, idx);
	idx += sizeof(index_reg_16);

	idx++[0] = static_cast<uint8_t>(stack.size());

	for (auto& prog_ctr : stack) {
		auto prog_ctr_16 = static_cast<uint16_t>(prog_ctr);
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
			idx++[0] = px_byte;
			px_byte = 0;
		}
	}

	// only adding updated memory 
	if (lowest_mem_addr_updated != 0xFFF) {
		idx++[0] = lowest_mem_addr_updated;

		for (size_t i = lowest_mem_addr_updated; i <= last_mem_addr; i++) {
			idx++[0] = memory[i];
		}
	}

	std::string sav_file_name = program_name + "_" + std::to_string(state_number) + ".sav";

	// will overwrite the file if it already exists
	auto file = fopen(sav_file_name.c_str(), "w");

	fprintf(file, "%s", buffer.data());
	fclose(file);

	return true;
}

void Chip8::load_program_state(std::string file_name) {

	if (file_name.size() == 0) {
		return;
	}

	auto file = fopen(file_name.c_str(), "r");

	if (file == NULL) {
		return;
	}

	fseek(file, 0L, SEEK_END);
	auto file_size = ftell(file);

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
	buffer_ptr += sizeof(uint32_t);

	program_ctr = sys_get_be16(buffer_ptr);
	buffer_ptr += sizeof(uint16_t);

	index_reg = sys_get_be16(buffer_ptr);
	buffer_ptr += sizeof(uint16_t);

	auto stack_size = buffer_ptr[0];
	buffer_ptr++;

	stack.clear();
	stack.reserve(stack_size);

	for (uint8_t i = 0; i < stack_size; i++) {
		uint16_t ptr_ctr = sys_get_be16(buffer_ptr);
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

	// if the rom never updated the memory then we don't try to overwrite the default memory values from the ROM
	if (static_cast<uint16_t>(buffer_ptr - buffer.data()) == file_size) {
		return;
	}

	buffer_ptr++;

	// if the amount of total bytes read from the buffer is less than the file size
	for (uint8_t i = 0; static_cast<uint16_t>(buffer_ptr - buffer.data()) < file_size; i++) {
		memory[0x200 + i] = buffer_ptr[0];
		buffer_ptr++;
	}
}