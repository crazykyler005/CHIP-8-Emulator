# pragma once

#include "chip8interpreter.hpp"

enum class ScrollDirection : uint8_t {
	DOWN = 0,
	LEFT,
	RIGHT,
	UP
};

class SuperChip : public Chip8Interpreter {

public:
	SuperChip(Chip8Type type);
	bool switch_type(Chip8Type type) override;

private:

	inline static uint8_t super_fontset[160] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0xF0, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 2
		0xF0, 0x10, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x10, 0xF0, 0xF0, // 3
		0x90, 0x90, 0x90, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 4
		0xF0, 0x80, 0x80, 0xF0, 0x10, 0x10, 0x10, 0x10, 0xF0, 0xF0, // 5
		0xF0, 0x80, 0x80, 0xF0, 0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, // 6
		0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 7
		0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, // 8
		0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0x10, 0x10, 0xF0, 0xF0, // 9
		0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // A
		0xE0, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x90, 0x90, 0xE0, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xE0, 0xE0, // D
		0xF0, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x80, 0xF0, 0xF0, // E
		0xF0, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80  // F
	};

	void scroll_screen(ScrollDirection direction, uint8_t px_shift = 4);
	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg);
	void update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height) override;
	bool _high_res_mode_en = false;

	// TODO: Define this
	void interpret_additional_data() override;

	void lores_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height);

	// TODO: save to file instead
	uint8_t user_flag_registers[8];
};