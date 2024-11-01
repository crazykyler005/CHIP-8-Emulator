
#pragma once

#include "chip8interpreter.hpp"

class Chip8 : public Chip8Interpreter {

public:
	Chip8(Chip8Type type);
	~Chip8() override = default;  // Default destructor

	bool wait_for_key_release = false;
	bool switch_type(Chip8Type type) override;

protected:
	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) override;
	std::vector<uint8_t> additional_data;

private:
	inline static const uint8_t SPRITE_PX_WIDTH = 8;

	void update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height) override;
	void interpret_additional_data() override { return; };
};
