
#include "chip8.hpp"

Chip8::Chip8(Chip8Type type) :
	Chip8Interpreter("Chip-8", type, 64, 32, 8, 15)
{
	_selected_planes = 0b01;
	_increment_i = true;
}

bool Chip8::switch_type(Chip8Type type)
{
	if (type != Chip8Type::ORIGINAL && type != Chip8Type::CHIP48) {
		return false;
	}
	
	_type = type;
	// _increment_i = (_type != Chip8Type::SUPER_1p1);

	return true;
}

void Chip8::update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height)
{
	// Reset register VF
	registers[0xF] = 0;

	// don't set the draw flag
	if (sprite_height == 0) {
		return;
	}
	// printf("x: %d, y: %d\n", x, y);

	// the starting x and y positions wrap
	x = x % native_width;
	y = y % native_height;

	// if the y position of the pixel is off the screen, stop drawing
	for (uint8_t yline = 0; yline < sprite_height && (y + yline) < native_height; yline++)
	{
		// Fetch the pixel value from the memory starting at location I
		uint8_t pixel = memory[index_reg + yline];

		// Loop over 8 bits of one row
		// if the x position of the pixel is off the screen, stop drawing
		for (int xline = 0; xline < SPRITE_PX_WIDTH && (x + xline) < native_width; xline++) {
			// Check if the current evaluated pixel is set to 1 (note that 0x80 >> xline scan through the byte, one bit at the time)
			if ((pixel & (0x80 >> xline)) != 0) {

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

bool Chip8::run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) {

	uint8_t low_byte = opcode & 0xFF;
	uint8_t low_nibble = opcode & 0xF;

	if (_type == Chip8Type::CHIP48) {
		// No operation
		switch (opcode & 0xF000) {
			case 0xB000:
				program_ctr = registers[VX_reg] + low_byte;
				break;

			case 0x8000:
				// Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
				if ((low_byte & 0xF) == 6) {
					auto previous_VX = registers[VX_reg];

					registers[VX_reg] >>= 1;
					registers[0xF] = previous_VX & 0x1;

				// Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
				} else if ((low_byte & 0xF) == 0xE) {
					auto previous_VX = registers[VX_reg];

					registers[VX_reg] <<= 1;
					registers[0xF] = (previous_VX & 0x80) ? 1 : 0;
				} else {
					return false;
				}

				break;

			case 0xF000:

				if (low_byte == 0x55) {
					for (uint8_t i = 0; i <= VX_reg; i++) {
						memory[index_reg + i] = registers[i];
					}

					if (index_reg < lowest_mem_addr_updated) {
						lowest_mem_addr_updated = index_reg;
					}

					if (_increment_i) {
						index_reg += VX_reg;
					}

				// Fills from V0 to VX (including VX) with values from memory, starting at address I.
				// The offset from I is increased by 1 for each value read, but I itself is left unmodified.
				} else if (low_byte == 0x65) {
					for (uint8_t i = 0; i <= VX_reg; i++) {
						registers[i] = memory[index_reg + i];
					}

					if (_increment_i) {
						index_reg += VX_reg;
					}
				}
				break;

			default:
				return false;
		}

	// COSMAC based variants will reset VF for opcodes 8XY1, 8XY2, 8XY3
	} else if (((opcode & 0xF000) == 0x8000) && (low_nibble < 4 && low_nibble > 0)) {
		registers[0xF] = 0;
		return false;
	} else {
		return false;
	}

	return true;
}