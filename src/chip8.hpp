
#pragma once

#include <stdint.h>
#include <string>
#include <cstdio>
#include <vector>

class Chip8 {

// typedef struct __attribute__((__packed__)) uint12 { 
// 	unsigned val:12 = 0;
// } uint12_t;

public:
	Chip8();

	void reset();
	void run();
	void update_gfx(uint8_t& x, uint8_t& y, uint8_t pix_height);

	bool load_program(std::string file_path);
	bool save_program_state(uint8_t state_number, uint32_t utc_timestamp);
	void load_program_state(std::string file_name);

	// 64 pixels width by 32 pixels height
	inline static const uint8_t native_width = 64;
	inline static const uint8_t native_height = 32;
	inline static const time_t MICRO_SECONDS_PER_FRAME = (1/60) * 1000000;
	inline static const std::string DEFAULT_TITLE = "Chip-8 Emulator";

	bool keys_pressed[16] = {};
	bool draw_flag = false;
	bool is_paused = false;
	bool _0xFX1E_overflow_enabled = false;
	bool play_sfx = false;

	// TODO: optimise how pixel states are stored by changing the above array to the following 
	// so that each bit is mapped to a pixel and each row of them is mapped to a 64bit variable
	// uint64_t px_states[32] = {};
	uint8_t px_states[64 * 32] = {};	
	uint8_t registers[16] = {};

private:
	void run_instruction();
	void countdown_timers();

	std::string program_name = "";
	inline static const std::string SAVE_FILE_EXTENSION = ".sav";
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	// 0x200-0xFFF - Program ROM and work RAM

	inline static const uint16_t MEMORY_SIZE = 4096; //0x1000

	inline static const uint16_t FONT_START_ADDR = 0x000;

	inline static const uint16_t call_stack_start = 0xEA0; // 0xEA0-0xEFF
	inline static const uint16_t call_stack_end = 0xEFF;

	// inline static const uint16_t refresh_display_addr_start = 0xF00;
	// inline static const uint16_t refresh_display_addr_end = 0xFFF;

	// most programs written for the original system begin at memory location
	// 0x200 because the interpreter occupied the first 512 bytes
	inline static const uint16_t program_start_addr = 0x200;

	inline static uint8_t fontset[80] =
	{ 
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	inline static const uint8_t SPRITE_PX_WIDTH = 8;

	uint8_t memory[4096] = {};

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack
	inline static size_t MAX_STACK_SIZE = 16;
	std::vector<uint16_t> stack;

	uint16_t index_reg = 0;
	uint16_t program_ctr = program_start_addr;

	// used to figure out if we should be saving data from memory into a save state
	uint16_t lowest_mem_addr_updated = 0xFFF;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;
};
