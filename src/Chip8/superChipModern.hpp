# pragma once

#include "superChipInterpreter.hpp"

class SuperChipModern : public SuperChipInterpreter {

public:
	SuperChipModern();
	~SuperChipModern() override { Chip8Interpreter::wait_for_display_update = true; };  // Default destructor

private:
	void low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height) override;
};