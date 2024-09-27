#include "chip8.hpp"

enum class ScrollDirection : uint8_t {
	DOWN = 0,
	LEFT,
	RIGHT,
	UP
};

class SuperChip {

public:
	inline static const std::string DEFAULT_TITLE = "Super Chip Emulator";

	// 64 pixels width by 32 pixels height
	inline static const uint8_t native_width = 128;
	inline static const uint8_t native_height = 64;

	void switch_type(Chip8Type type);

protected:
	void update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height);
	// uint8_t px_states[native_width * native_height];

private:
	inline static const uint8_t SPRITE_PX_WIDTH = 16;

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

	void scroll_screen(ScrollDirection direction, uint8_t px_shift);
	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg);
	bool high_res_mode_en = false;

	void interrupt_additional_data() = 0;

	uint8_t user_flag_registers[8];
};