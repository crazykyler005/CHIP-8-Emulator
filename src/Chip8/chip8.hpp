
#pragma once

#include "chip8interpreter.hpp"

class Chip8 : Chip8Interpreter {

public:
	Chip8();

	bool wait_for_key_release = false;
	bool _0xFX1E_overflow_enabled = false;

	bool switch_type(Chip8Type type) override;

protected:
	Chip8Type _type = Chip8Type::ORIGINAL;

	// Before the CHIP-8 interpreters CHIP48 and SUPER-CHIP (1970s - 1980s), the I register
	// was incremented each time it stored or loaded one register. (I += X + 1).
	bool increment_i = true;
	bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) override;
	std::vector<uint8_t> additional_data;

private:
	std::string program_name = "";
	inline static const std::string SAVE_FILE_EXTENSION = ".sav";
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	// 0x200-0xFFF - Program ROM and work RAM

	inline static const uint16_t MEMORY_SIZE = 4096; //0x1000

	inline static const uint16_t FONT_START_ADDR = 0x000;

	// most programs written for the original system begin at memory location
	// 0x200 because the interpreter occupied the first 512 bytes
	inline static const uint16_t PROGRAM_START_ADDR = 0x200;

	uint8_t memory[4096] = {};
	uint8_t registers[16] = {};

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack
	inline static size_t MAX_STACK_SIZE = 16;
	std::vector<uint16_t> stack;

	uint16_t index_reg = 0;
	uint16_t program_ctr = PROGRAM_START_ADDR;

	// used to figure out if we should be saving data from memory into a save state
	uint16_t lowest_mem_addr_updated = 0xFFF;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;

	void update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height) override;
	void cancel_key_wait_event();
	void interrupt_additional_data() override { return; };
};
