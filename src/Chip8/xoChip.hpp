#pragma once

#include "superChipInterpreter.hpp"

class XOChip : public SuperChipInterpreter {

public:
	XOChip();
	~XOChip() override { Chip8Interpreter::wait_for_display_update = true; };  // Default destructor

private:
	// Skip instructions will skip over the entire double-wide F000 NNNN instruction.
	void skip_instruction() override;
	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) override;
	void update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height) override;
	void low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height) override;
	
	uint8_t DEFAULT_TOTAL_DRAWING_PLANES = 2;
	uint16_t _pitch_reg = 0;
};