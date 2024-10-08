
#include "chip8.hpp"

Chip8::Chip8() : 
	Chip8Interpreter(64, 32, 8)
{
	INTERPRETER_NAME = "Chip-8";
	px_states.resize(native_width * native_height);
}

bool Chip8::switch_type(Chip8Type type)
{
	if (type != Chip8Type::ORIGINAL && type != Chip8Type::AMIGA_CHIP8) {
		printf("Invalid type conversion\n");
		return false;
	}
	
	_type = type;
	// increment_i = (_type != Chip8Type::SUPER_1p1);

	return true;
}

void Chip8::update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height) {
	// Reset register VF
	registers[0xF] = 0;
	// printf("x: %d, y: %d\n", x, y);

	// the starting x and y positions wrap
	x = x % native_width;
	y = y % native_height;

	// if the y position of the pixel is off the screen, stop drawing
	for (uint8_t yline = 0; yline < pix_height && (y + yline) < native_height; yline++)
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

	if (_type == Chip8Type::AMIGA_CHIP8) {
		// No operation
		if (opcode == 0x0000) {
			;

		// Stop
		} else if (opcode == 0xF000) {
			is_running = false;
			
		// Modified instruction: FX1E - If the result of VX+I overflows set VF to 1
		// There is one known game that depends on this modified behavior happening.
		} else if (opcode & 0xF0FF == 0xF01E) {
			index_reg += registers[VX_reg];
			registers[0xF] = (index_reg & 0xF000) ? 1 : 0;
		} else {
			return false;
		}
	} else {
		return false;
	}

	return true;
}