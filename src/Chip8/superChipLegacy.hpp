# pragma once

#include "superChipInterpreter.hpp"

class SuperChipLegacy : public SuperChipInterpreter {

public:
	SuperChipLegacy(Chip8Type type);
	~SuperChipLegacy() override = default;  // Default destructor

	bool switch_type(Chip8Type type) override;

private:
	void low_res_draw_gfx(uint8_t& x, uint8_t& y, uint8_t& sprite_height) override;

	// TODO: Define this
	void interpret_additional_data() override;

	inline static const uint16_t SC_V1p0_OPS = 900;
	inline static const uint16_t SC_V1p1_OPS = 1800;
};