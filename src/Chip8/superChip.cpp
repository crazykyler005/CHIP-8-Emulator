#include "superChip.hpp"

SuperChip::SuperChip(Chip8Type type) : 
	Chip8Interpreter("Super Chip", 128, 64, 16)
{
	_type = type;

	// loading high res fontset into the designated position in memory (81-240)
	std::copy(std::begin(super_fontset), std::end(super_fontset), std::begin(memory) + sizeof(fontset));

	opcodes_per_second = 1800;
	increment_i = (_type != Chip8Type::SUPER_1p1);
}

bool SuperChip::switch_type(Chip8Type type)
{
	if (type != Chip8Type::SUPER_1p0 && type != Chip8Type::SUPER_1p1) {
		printf("Invalid type conversion\n");
		return false;
	}
	
	_type = type;
	increment_i = (_type != Chip8Type::SUPER_1p1);

	return true;
}

void SuperChip::update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height) {
	// used to determine if a pixel is represented by x on screen pixels
	uint8_t pixel_size = 1;

	// Reset register VF
	registers[0xF] = 0;

	// the starting x and y positions wrap
	if (_high_res_mode_en) {
		x = x % native_width;
		y = y % native_height;

	// low-resolution mode (64x32), even though the application is suppose to emulate that the
	// native resolution (128x64) does not change thus the X & Y coordinates are doubled and 
	// each pixel is represented by 2x2 on-screen pixels.
	} else {
		x = (x * 2) % native_width;
		y = (y * 2) % native_height;

		pixel_size = 2;
	}

	for (uint8_t yline = 0; yline < sprite_height; yline++)
	{
		// Fetch the pixels from the memory starting at location I
		uint16_t pixels = 0;

		if (!_high_res_mode_en) {
			// in lores, each sprite has a width of 8px so only a single byte from mem is grabbed per row of pixels.
			for (uint8_t z = 0; z < (SPRITE_PX_WIDTH / 2); z++) {
				if ((memory[index_reg + yline] & (1 << z))) {
					// 8px row is upscaled to 16px
					// ex: 10111101 -> 11001111 11110011
					pixels = (0b11 << (z*2)) | pixels;
				}
			}

		} else {
			pixels = memory[index_reg + yline] << 8 + memory[index_reg + yline + 1];
		}

		for (int xline = 0; xline < SPRITE_PX_WIDTH && (x + xline) < native_width; xline++) {
			if ((pixels & (0x8000 >> xline)) != 0) {

				// drawing a 2x2 pixel if low res mode is enabled instead of a 1x1
				for (uint8_t i = 0; i < pixel_size; i++) {
					// if the y position of the pixel is off the screen, stop drawing
					if ((y + yline + i) >= native_height) {
						break;
					}

					size_t current_pixel = (x + xline + ((y + i + (yline * 2)) * native_width));

					// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
					if (px_states[current_pixel] == 1) {
						registers[0xF] = 1;
					}

					// Set the pixel value by using XOR
					px_states[current_pixel] ^= 1;
				}
			}
		}
	}

	draw_flag = true;
	
	return;
}

void SuperChip::scroll_screen(ScrollDirection direction, uint8_t px_shift)
{
	if (_high_res_mode_en) {
		px_shift = px_shift * 2;
	}

	switch (direction)  {
		case ScrollDirection::DOWN:
			for (uint8_t yline = native_height - px_shift; yline > 0; yline--) {
				for (uint8_t xline = 0; xline < native_width; xline++)
				{
					px_states[xline + ((yline + px_shift) * native_width)] = px_states [xline + (yline * native_width)];
				}
			}

			// clear everything above moved pixels
			memset(px_states.data(), 0, (native_width * px_shift));
			break;

		case ScrollDirection::RIGHT:
			for (uint8_t xline = native_width - px_shift; xline > 0; xline--) {
				for (uint8_t yline = 0; yline < native_height; yline++)
				{
					px_states[(xline + px_shift) + (yline * native_width)] = px_states [xline + (yline * native_width)];

					// Clear everything to the left of the shifted pixels
					if (xline < px_shift) {
						px_states [xline + (yline * native_width)] = false;
					}
				}
			}

			break;

		case ScrollDirection::LEFT:
			for (uint8_t xline = 0; xline < native_width - px_shift; xline++) {
				for (uint8_t yline = 0; yline < native_height; yline++)
				{
					px_states[xline + (yline * native_width)] = px_states [(xline + px_shift) + (yline * native_width)];

					// Clear everything to the right of the shifted pixels
					if (xline >= (native_width - px_shift)) {
						px_states [(xline + px_shift) + (yline * native_width)] = false;
					}
				}
			}

			break;

		// this scroll operation is not officially supported by super chip 1.1
		case ScrollDirection::UP:
			for (uint8_t yline = px_shift; yline < native_height; yline++) {
				for (uint8_t xline = 0; xline < native_width; xline++)
				{
					px_states[xline + (yline * native_width)] = px_states [xline + (native_width * (yline - px_shift))];
				}
			}

			// clear everything below moved pixels
			memset(&memory[native_width * (native_height - px_shift)], false, (native_width * px_shift));

			break;

		default:
			return;
	}

	draw_flag = true;
}

bool SuperChip::run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg)
{
	uint16_t sub_opcode = (opcode & 0xFFF);
	uint8_t low_byte = (opcode & 0xFF);

	switch (opcode & 0xF000)
	{
		case 0x0000:

			if (_type == Chip8Type::SUPER_1p1) {
				// Scroll display N pixels up; in low resolution mode, N/2 pixels
				// this instruction is not officially supported by super chip 1.1
				if (sub_opcode == 0xB0) {
					scroll_screen(ScrollDirection::UP, sub_opcode & 0xF);

				// Scroll display N pixels down; in low resolution mode, N/2 pixels
				} else if ((sub_opcode & 0xFF0) == 0xC0) {
					// in the legacy superchip (v1.0-v1.1), 00C0 is an invalid instruction and would crash the application.
					scroll_screen(ScrollDirection::DOWN, low_byte & 0xF);

				// Scroll right by 4 pixels; in low resolution mode, 2 pixels
				} else if (sub_opcode == 0xFB) {
					scroll_screen(ScrollDirection::RIGHT);

				// Scroll left by 4 pixels; in low resolution mode, 2 pixels
				} else if (sub_opcode == 0xFC) {
					scroll_screen(ScrollDirection::LEFT);
				} else {
					return false;
				}

			// Exit interpreter
			} else if (sub_opcode == 0xFD) {
				is_running = false;

			} else if (sub_opcode == 0xFE) {
				_high_res_mode_en = false;

			} else if (sub_opcode == 0xFF) {
				_high_res_mode_en = true;
				
			} else {
				return false;
			}

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

		case 0xB000: // PC = VX + NNN
			// CHIP-48 and legacy SUPER-CHIP version (possible bug)
			program_ctr = registers[VX_reg] + sub_opcode;
			return true;

		case 0xD000:

			if (low_byte & 0x0F == 0) {
				update_gfx(registers[VX_reg], registers[VY_reg], (_high_res_mode_en ? 16 : 8));
			} else {
				return false;
			}

			break;

		case 0xF000:

			// A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event)
			if (sub_opcode == 0x00A) {
				bool key_pressed = false;

				for (uint8_t i = 0; i < keys.size(); i++) {
					// if two key presses happen at the same time, the lowest order press gets registered
					if (keys[i].is_pressed) {
						key_pressed = true;
						registers[VX_reg] = i;
					}
				}

				if (!key_pressed) {
					return true;
				}

			} else if (low_byte == 0x29 && _type == Chip8Type::SUPER_1p0) {

				if (registers[VX_reg] & 0xF0 == 1) {
					registers[VX_reg] * 10 + sizeof(fontset);
				}
				
				index_reg = registers[VX_reg] * 5;

			// Sets I to a large hexadecimal character based on the value of VX. Characters 0-F (in hexadecimal) are represented by a 8x10 font
			} else if (low_byte == 0x30 && _type == Chip8Type::SUPER_1p1) {
				index_reg = registers[VX_reg] * 10 + sizeof(fontset);

			} else if (low_byte == 0x75) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					user_flag_registers[i] = registers[i];
				}

			} else if (low_byte == 0x85) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					registers[i] = user_flag_registers[i];
				}

			} else {
				return false;
			}

			break;

		default:
			return false;
	}

	program_ctr += 2;
	return true;
}

// TODO: Define this
void SuperChip::interpret_additional_data()
{
	return;
}