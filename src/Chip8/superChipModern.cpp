#include "superChipModern.hpp"

SuperChipModern::SuperChipModern() :
	SuperChipInterpreter("Superchip Modern", Chip8Type::SUPER_MODERN, 30)
{
	Chip8Interpreter::wait_for_display_update = false;
	_user_flag_registers.resize(16);

	_increment_i = false;
}

void SuperChipModern::low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height)
{
	// printf("width: %d, height: %d, x: %d, y: %d, px_height: %d\n", native_width, native_height, x, y, sprite_height);
	
	// low-resolution mode (64x32), even though the application is suppose to emulate that the
	// native resolution (128x64) does not change thus the X & Y coordinates are doubled and 
	// each pixel is represented by 2x2 on-screen pixels.

	const uint8_t pixel_size = 2;
	const uint8_t sprite_width = 8;
	const uint8_t draw_width = 16;

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

			if ((pixels & (0x8000 >> xline)) != 0) {
				// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
				if (px_states[current_pixel] == 1) {
					registers[0xF] = 1;
				}

				if (xline % 2 == 0) {
					px_states[current_pixel] ^= 1;
				} else {
					// copying the state of the previous equated XOR'd pixel to the
					// right of it thus upscaling it horizontally to 2x1 on-screen pixels
					px_states[current_pixel] = px_states[current_pixel - 1];
				}
			}

			// upscaling current on-screen pixel vertically to 1x2 on-screen pixels
			if (((current_y_pos + 1) < native_height)) {
				px_states[current_pixel + native_width] = px_states[current_pixel];
			}
		}
	}

	draw_flag = true;
	
	return;
}