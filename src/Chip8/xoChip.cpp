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

		for (uint8_t xline = 0; xline < draw_width; xline++) {
			if ((pixels & (0x8000 >> xline)) != 0) {

				uint8_t wrapped_y = (y + yline) % native_height;
				uint8_t wrapped_x = (x + xline) % native_width;

				// sets pixels in each selected plane
				for (uint8_t plane = 0; plane < number_of_planes(); plane++) {
					if ((_selected_planes & (1 << plane)) == 0) {
						continue;
					}

					size_t plane_offset = (native_height * native_width * plane);
					size_t current_pixel = (wrapped_x + (wrapped_y * native_width)) + plane_offset;

					// In high resolution mode, DXYN/DXY0 sets VF to the number of rows that either collide 
					// with another sprite or are clipped by the bottom of the screen
					if (px_states[current_pixel] == 1) {
						registers[0xF] = 1;
					}

					px_states[current_pixel] ^= 1;
				}
			}
		}
	}

	draw_flag = true;
	
	return;
}

void XOChip::low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height)
{
	// low-resolution mode (64x32), even though the application is suppose to emulate that the
	// native resolution (128x64) does not change thus the X & Y coordinates are doubled and 
	// each pixel is represented by 2x2 on-screen pixels.
	bool large_sprite = (sprite_height == 16);

	const uint8_t pixel_size = 2;
	uint8_t sprite_width = large_sprite ? sprite_height : 8;
	uint8_t draw_width = large_sprite ? 32 : 16;

	// printf("width: %d, height: %d, x: %d, y: %d, px_height: %d\n", native_width, native_height, x, y, sprite_height);
	x = (x * 2) % native_width;
	y = (y * 2) % native_height;

	// if the y position of the pixel is off screen, stop drawing
	for (uint8_t yline = 0; yline < (sprite_height * 2); yline += pixel_size)
	{
		uint32_t pixels = 0;

		// if low-res, each sprite has a width of 8px so only a single byte from mem is pulled per row of pixels.
		for (uint8_t px = 0; px < sprite_width; px++) {

			for (uint8_t px = 0; px < sprite_width; px++) {
				pixels <<= pixel_size;

				// Fetch the pixels from the memory starting at location I
				uint8_t current_byte = memory[index_reg + (px / 8) + (large_sprite ? yline : yline / 2)];
        		uint8_t px_mask = 0b10000000 >> (px % 8);
				
				if (current_byte & px_mask) {
					// 8px sprite row is upscaled to 16px
					// ex: 10111101 -> 11001111 11110011
					pixels |= 0b11;
				}
			}
		}

		// if the x position of a pixel is off screen, stop drawing
		for (uint xline = 0; xline < draw_width; xline++) {

			uint8_t wrapped_y = (y + yline) % native_height;
			uint8_t wrapped_x = (x + xline) % native_width;

			// sets pixels in each selected plane
			for (uint8_t plane = 0; plane < number_of_planes(); plane++) {
				if ((_selected_planes & (1 << plane)) == 0) {
					continue;
				}

				size_t plane_offset = (native_height * native_width * plane);
				size_t current_pixel = (wrapped_x + (wrapped_y * native_width)) + plane_offset;

				if ((pixels & (0x1 << ((draw_width - 1) - xline))) != 0) {
					// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
					if (px_states[current_pixel] == 1) {
						registers[0xF] = 1;
					}

					if (xline % 2 == 0) {
						px_states[current_pixel] ^= 1;
					} else {
						// copying the state of the previous equated XOR'd pixel to the
						// right of it thus upscaling it horizontally to 2x1 on-screen pixels
						px_states[current_pixel] = px_states[(current_pixel - 1)];
					}
				}

				// upscaling current on-screen pixel vertically to 1x2 on-screen pixels
				px_states[current_pixel + native_width] = px_states[current_pixel];
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

			if (opcode == 0x00E0) {

				for (uint8_t plane = 0; plane < number_of_planes(); plane++) {
					if ((_selected_planes & (1 << plane)) == 0) {
						continue;
					}

					memset(px_states.data() + (native_height * native_width * plane), 0, native_height * native_width);
					printf("cleared pixels in plane %d\n", (plane + 1));

					draw_flag = true;
				}

			// Scroll display N pixels up; in low resolution mode, N/2 pixels
			} else if ((sub_opcode & 0xFF0) == 0xD0) {
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

				// clearing screen in all planes
				memset(px_states.data(), 0, px_states.size());

			} else if (sub_opcode == 0xFF) {
				_high_res_mode_en = true;

				// clearing screen in all planes
				memset(px_states.data(), 0, px_states.size());
				is_paused = true;
				
			} else {
				return false;
			}

			break;

		case 0x5000:
			// Save VX..VY to memory starting at I; does not increment I
			if ((low_byte & 0xF) == 2) {
				const uint8_t dist = std::abs(VX_reg - VY_reg) + 1;

				if (VX_reg < VY_reg) {
					for (uint8_t i = 0; i < dist; i++) {
						memory[index_reg + i] = registers[VX_reg + i];
					}
				} else {
					// Save to memory in reversed order
					for (uint8_t i = 0; i < dist; i++) {
						memory[index_reg + i] = registers[VX_reg - i];
					}
				}

			// Load VX..VY from memory starting at I; does not increment I
			} else if ((low_byte & 0xF) == 3) {

				const uint8_t dist = std::abs(VX_reg - VY_reg) + 1;

				if (VX_reg < VY_reg) {
					for (uint8_t i = 0; i < dist; i++) {
						registers[VX_reg + i] = memory[index_reg + i];
					}
				} else {
					// Load from memory in reversed order
					for (uint8_t i = 0; i < dist; i++) {
						registers[VX_reg - i] = memory[index_reg + i];
					}
				}

			} else {
				return false;
			}

			break;

		case 0xD000:
			if ((low_byte & 0x0F) == 0) {
				update_gfx(registers[VX_reg], registers[VY_reg], 16);
			} else {
				update_gfx(registers[VX_reg], registers[VY_reg], static_cast<uint8_t>(opcode & 0xF));
			}

			break;

		case 0xF000:

			// F000 NNNN: Load I with 16-bit address NNNN
			if (sub_opcode == 0x000) {
				index_reg = static_cast<uint16_t>(memory[program_ctr + 2] << 8 | memory[program_ctr + 3]);
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
	uint16_t next_opcode = static_cast<uint16_t>(memory[program_ctr + 2] << 8 | memory[program_ctr + 3]);
	program_ctr += ((next_opcode == 0xF000) ? 4 : 2);
};