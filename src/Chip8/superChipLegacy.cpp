#include "superChipLegacy.hpp"

SuperChipLegacy::SuperChipLegacy(Chip8Type type) : 
	SuperChipInterpreter("Superchip Legacy", type, (type == Chip8Type::SUPER_1p0) ? SC_V1p0_OPF : SC_V1p1_OPF)
{
	_selected_planes = 0b01;

	// loading high res fontset into the designated position in memory (81-240)
	std::copy(std::begin(super_fontset), std::end(super_fontset), std::begin(memory) + sizeof(fontset));

	_increment_i = (_type != Chip8Type::SUPER_1p1);
}

bool SuperChipLegacy::switch_type(Chip8Type type)
{
	if (_type == Chip8Type::SUPER_MODERN) {
		return false;
	}

	if (type != Chip8Type::SUPER_1p0 && type != Chip8Type::SUPER_1p1) {
		return false;
	}

	_type = type;
	opcodes_per_frame = (_type == Chip8Type::SUPER_1p0) ? SC_V1p0_OPF : SC_V1p1_OPF;

	_increment_i = (_type != Chip8Type::SUPER_1p1);

	return true;
}

void SuperChipLegacy::low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height)
{
	// printf("width: %d, height: %d, x: %d, y: %d, px_height: %d\n", native_width, native_height, x, y, sprite_height);
	
	// low-resolution mode (64x32), even though the application is suppose to emulate that the
	// native resolution (128x64) does not change thus the X & Y coordinates are doubled and 
	// each pixel is represented by 2x2 on-screen pixels.

	const uint8_t pixel_size = 2;
	const uint8_t sprite_width = 8;
	const uint8_t draw_width = 32;

	x = (x * 2) % native_width;
	y = (y * 2) % native_height;

	// the initial drawing starts at the lowest divisible horizontal 16th pixel border
	uint8_t starting_x = x & 0xF0;

	// if the y position of the pixel is off screen, stop drawing
	for (uint8_t yline = 0; yline < (sprite_height * 2) && (y + yline) < native_height; yline += pixel_size)
	{
		uint8_t current_y_pos = y + yline;

		// A row of pixels are always drawn in 32x2 pixel strips thus a 32px buffer is needed
		uint32_t pixels = 0;

		// if low-res, each sprite has a width of 8px so only a single byte from mem is pulled per row of pixels.
		for (uint8_t px = 0; px < sprite_width; px++) {
			// Fetch the pixels from the memory starting at location I
			if ((memory[index_reg + (yline / 2)] & (1 << px))) {
				// 8px sprite row is upscaled to 16px
				// ex: 10111101 -> 11001111 11110011
				pixels = (0b11 << (px * 2)) | pixels;
			}
		}

		// shifts sprite pixels to the right of the 16th pixel border
		pixels <<= ((draw_width / 2) - (x & 0xF));

		// if the x position of a pixel is off screen, stop drawing
		for (uint xline = 0; xline < draw_width && (starting_x + xline) < native_width; xline++) {

			size_t current_pixel = (starting_x + xline + (current_y_pos * native_width));

			if ((pixels & (0x80000000 >> xline)) != 0) {
				// Check if the pixel on the display is set to 1. If it is set, we need to register the collision by setting the VF register
				if (px_states[current_pixel] == 1) {
					registers[0xF] = 1;
				}

				// even the pixels that are not a part of the sprite get XOR'd with the on-screen pixels which
				// could create unintended artifacts though these are not known to be seen in created applications
				px_states[current_pixel] ^= 1;
			}

			// Each of the 32 pixels per row drawn gets upscaled vertically (1x1->1x2)

			// NOTE: even though each pixel of a sprite is represented as 2x2 on screen pixels, 
			// that doesn't mean the state of those 4 pixels are going to be the same. The superchip XORs the
			// the top pixels with the current on-screen pixels and then copies them down.
			if ((current_y_pos + 1) < native_height) {
				px_states[current_pixel + native_width] = px_states[current_pixel];
			}
		}
	}

	draw_flag = true;
	
	return;
}

// TODO: Define this
void SuperChipLegacy::interpret_additional_data()
{
	return;
}