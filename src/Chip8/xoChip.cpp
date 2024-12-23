#include "xoChip.hpp"

// Altered instructions
// Clear, scroll and draw instructions only apply to the selected drawing plane.

// Compatibility notes
// Switching resolution mode erases the screen, unlike on SCHIP.
// Audio patterns are a series of 1-bit samples played at a rate of 4000*2^((pitch-64)/48) Hertz, or samples per second, where pitch is the pitch register.
// The pitch register is initialized to 64, making the default sample rate 4000 Hz. audio pattern playback rate to 4000*2^((VX-64)/48)Hz.
// The audio pattern buffer is restricted to 16 bytes in Octo.
// The audio pattern buffer is not necessarily cleared on program start, although Octo does so.
// The audio pattern buffer is loaded when F002 is called. Subsequent rewrites of the memory that I pointed to at that time are not reflected in the buffer.
// The playback offset of the audio pattern buffer only resets when the sound timer reaches 0 (either by itself, or by being set explicitly).
XOChip::XOChip() :
	SuperChipInterpreter("XO Chip" , Chip8Type::XO, 1800)
{
	Chip8Interpreter::wait_for_display_update = false;

	// default ONLY for XO-chip
	_selected_planes = 1;

	px_states.resize(static_cast<size_t>(native_height * native_width * DEFAULT_TOTAL_DRAWING_PLANES));
}

void XOChip::update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height)
{
	// Reset register VF
	registers[0xF] = 0;

	if (!_high_res_mode_en) {
		low_res_draw_gfx(x, y, sprite_height);
		return;
	}

	// if a sprites height is 16px then it's width is also 16px otherwise it's 8px
	uint8_t draw_width = (sprite_height == 16) ? sprite_height : 8;

	// the starting x and y positions wrap
	// printf("width: %d, height: %d, x: %d, y: %d, px_height: %d\n", native_width, native_height, x, y, sprite_height);
	x = x % native_width;
	y = y % native_height;

	for (uint8_t yline = 0; yline < sprite_height; yline++)
	{
		uint16_t pixels = 0;

		// Fetch the pixels from the memory starting at location I
		if (draw_width != 16) {
			pixels = (static_cast<uint16_t>(memory[index_reg + yline]) << 8);
		} else {
			pixels = (static_cast<uint16_t>(memory[index_reg + (yline * 2)]) << 8) + memory[index_reg + (yline * 2) + 1];
		}

		bool collision_in_row = false;

		for (uint8_t xline = 0; xline < draw_width; xline++) {
			if ((pixels & (0x8000 >> xline)) != 0) {

				// if the x or y position of the pixel is off the screen, stop drawing
				if (((y + yline) >= native_height) || ((x + xline) >= native_width)) {
					registers[0xF]++;
					break;
				}

				size_t current_pixel = (x + xline + ((y + yline) * native_width));

				// sets pixels in each selected plane
				for (uint8_t plane = 1; plane < (number_of_planes() + 1); plane++) {
					if ((_selected_planes & (1 << (plane - 1))) == 0) {
						continue;
					}

					// In high resolution mode, DXYN/DXY0 sets VF to the number of rows that either collide 
					// with another sprite or are clipped by the bottom of the screen
					if (px_states[current_pixel * plane] == 1 && !collision_in_row) {
						if (!collision_in_row) {
							registers[0xF]++;
						}

						collision_in_row = true;
					}

					px_states[current_pixel * plane] ^= 1;
				}
			}
		}
	}

	if (registers[0xF] > 0) {
		registers[0xF] = 1;
	}

	draw_flag = true;
	
	return;
}

void XOChip::low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height)
{
	// low-resolution mode (64x32), even though the application is suppose to emulate that the
	// native resolution (128x64) does not change thus the X & Y coordinates are doubled and 
	// each pixel is represented by 2x2 on-screen pixels.

	const uint8_t pixel_size = 2;
	const uint8_t sprite_width = 8;
	const uint8_t draw_width = 16;

	// printf("width: %d, height: %d, x: %d, y: %d, px_height: %d\n", native_width, native_height, x, y, sprite_height);
	x = (x * 2) % native_width;
	y = (y * 2) % native_height;

	// if the y position of the pixel is off screen, stop drawing
	for (uint8_t yline = 0; yline < (sprite_height * 2) && (y + yline) < native_height; yline += pixel_size)
	{
		uint8_t current_y_pos = y + yline;
		uint16_t pixels = 0;

		// if low-res, each sprite has a width of 8px so only a single byte from mem is pulled per row of pixels.
		for (uint8_t px = 0; px < sprite_width; px++) {
			// Fetch the pixels from the memory starting at location I
			if ((memory[index_reg + (yline / 2)] & (1 << px))) {
				// 8px sprite row is upscaled to 16px
				// ex: 10111101 -> 11001111 11110011
				pixels = (0b11 << (px * 2)) | pixels;
			}
		}

		// if the x position of a pixel is off screen, stop drawing
		for (uint xline = 0; xline < draw_width && (x + xline) < native_width; xline++) {

			size_t current_pixel = (x + xline + (current_y_pos * native_width));

			// sets pixels in each selected plane
			for (uint8_t plane = 1; plane < (number_of_planes() + 1); plane++) {
				if ((_selected_planes & (1 << (plane - 1))) == 0) {
					continue;
				}

				if ((pixels & (0x8000 >> xline)) != 0) {
					// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
					if (px_states[current_pixel * plane] == 1) {
						registers[0xF] = 1;
					}

					if (xline % 2 == 0) {
						px_states[current_pixel * plane] ^= 1;
					} else {
						// copying the state of the previous equated XOR'd pixel to the
						// right of it thus upscaling it horizontally to 2x1 on-screen pixels
						px_states[current_pixel * plane] = px_states[(current_pixel - 1) * plane];
					}
				}

				// upscaling current on-screen pixel vertically to 1x2 on-screen pixels
				if (((current_y_pos + 1) < native_height)) {
					px_states[(current_pixel + native_width) * plane] = px_states[current_pixel * plane];
				}
			}
		}
	}

	draw_flag = true;
	
	return;
}

bool XOChip::run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg)
{
	uint16_t sub_opcode = (opcode & 0xFFF);
	uint8_t low_byte = (opcode & 0xFF);

	switch (opcode & 0xF000)
	{
		case 0x0000:

			// Scroll display N pixels up; in low resolution mode, N/2 pixels
			if (sub_opcode == 0xB0) {
				scroll_screen(ScrollDirection::UP, low_byte & 0xF);

			// Scroll display N pixels down; in low resolution mode, N/2 pixels
			} else if ((sub_opcode & 0xFF0) == 0xC0) {
				scroll_screen(ScrollDirection::DOWN, low_byte & 0xF);

			// Scroll right by 4 pixels; in low resolution mode, 2 pixels
			} else if (sub_opcode == 0xFB) {
				scroll_screen(ScrollDirection::RIGHT);

			// Scroll left by 4 pixels; in low resolution mode, 2 pixels
			} else if (sub_opcode == 0xFC) {
				scroll_screen(ScrollDirection::LEFT);

			} else if (sub_opcode == 0xFD) {
				is_running = false;

			// 00FE and 00FF, which switch between low and high resolution, will clear the screen as well.
			} else if (sub_opcode == 0xFE) {
				_high_res_mode_en = false;
				//memset(px_states.data(), 0, px_states.size());

			} else if (sub_opcode == 0xFF) {
				_high_res_mode_en = true;
				//memset(px_states.data(), 0, px_states.size());
				
			} else {
				return false;
			}

			break;

		case 0x5000:
			// Save VX..VY to memory starting at I; does not increment I
			if ((low_byte & 0xF) == 2) {
				for (uint8_t i = VX_reg; i < VY_reg; i++) {
					memory[index_reg + i] = registers[i];
				}

			// Load VX..VY from memory starting at I; does not increment I
			} else if ((low_byte & 0xF) == 3) {
				for (uint8_t i = VX_reg; i < VY_reg; i++) {
					registers[i] = memory[index_reg + i];
				}
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
			if ((low_byte & 0x0F) == 0) {
				update_gfx(registers[VX_reg], registers[VY_reg], (_high_res_mode_en ? 16 : 8));
			} else {
				update_gfx(registers[VX_reg], registers[VY_reg], static_cast<uint8_t>(opcode & 0xF));
			}

			break;

		case 0xF000:

			// F000 NNNN: Load I with 16-bit address NNNN
			if (sub_opcode == 0x000) {
				index_reg = memory[program_ctr + 2] + memory[program_ctr + 3];
				skip_instruction();

			// FN01: Select drawing planes by their corresponding bitmasks (0 for no planes)
			// There is only suppose to be two drawing planes but there is techically 2 extra unused plane bitmasks 4 and thus the potiental to have 16 color applications
			} else if (low_byte == 0x01) {

				_selected_planes = VX_reg;

				// uint8_t last_plane = 0;
				
				// // grabbing the most significant bit
				// for (uint8_t i = 3; i == 0; i--) {
				// 	if (_selected_planes & (1 << i)) {
				// 		last_plane = i + 1;
				// 		break;
				// 	}
				// }

				// if ((px_states.size() / (native_height * native_width)) < last_plane ) {
				// 	px_states.resize(native_height * native_width * last_plane);
				// }

			// F002: Store 16 bytes in audio pattern buffer, starting at I, to be played by the sound buzzer
			} else if (low_byte == 0x02) {
				// for (uint8_t i = 0; i < 16; i++) {
				// 	_audio_buffer[i] = memory[index_reg + i];
				// }

			// A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event)
			} else if (sub_opcode == 0x00A) {
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

			// Sets I to a large hexadecimal character based on the value of VX. Characters 0-F (in hexadecimal) are represented by a 8x10 font
			} else if (low_byte == 0x30) {
				index_reg = registers[VX_reg] * 10 + sizeof(fontset);

			// FX3A: Set the pitch register to the value in VX.
			} else if (low_byte == 0x3A) {
				_pitch_reg = registers[VX_reg];

			} else if (low_byte == 0x75) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					_user_flag_registers[i] = registers[i];
				}

			} else if (low_byte == 0x85) {
				for (uint8_t i = 0; i <= VX_reg; i++) {
					registers[i] = _user_flag_registers[i];
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

void XOChip::skip_instruction()
{
	// Skip instructions will skip over the entire double-wide F000 NNNN instruction.
	uint16_t next_opcode = (static_cast<uint16_t>(memory[program_ctr + 2]) << 8) + memory[program_ctr + 3];
	program_ctr += ((next_opcode == 0xF000) ? 4 : 2);
};