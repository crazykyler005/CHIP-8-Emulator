
#pragma once

#include "chip8interpreter.hpp"

class Chip8 : public Chip8Interpreter {

public:
	Chip8();

	bool wait_for_key_release = false;
	bool _0xFX1E_overflow_enabled = false;

	bool switch_type(Chip8Type type) override;

protected:
	Chip8Type _type = Chip8Type::ORIGINAL;

	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) override;
	std::vector<uint8_t> additional_data;

private:
	void update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height) override;
	void interrupt_additional_data() override { return; };
};
