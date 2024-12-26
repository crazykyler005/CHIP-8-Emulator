#pragma once

#include <stdint.h>
#include <string>
#include <cstdio>
#include <vector>

#include <SDL2/SDL.h>
#include <array>

enum class Chip8Type : uint8_t {
	ORIGINAL = 0,
	AMIGA_CHIP8,
	// CHIP48,
	SUPER_1p0,
	SUPER_1p1,
	SUPER_MODERN,
	XO,
	END
};

class Chip8Interpreter {

// typedef struct __attribute__((__packed__)) uint12 { 
// 	unsigned val:12 = 0;
// } uint12_t;

struct key_info {
	SDL_Scancode map; 
	bool is_pressed = false;
	bool released_on_wait_event = false;
};

public:
	Chip8Interpreter(std::string name, Chip8Type type, uint8_t width, uint8_t height, uint8_t sprite_width, uint16_t ops);
	virtual ~Chip8Interpreter() = default;

	const uint8_t native_width;
	const uint8_t native_height;

	virtual bool switch_type(Chip8Type type) { return (_type == type); };
	virtual Chip8Type& get_type() { return _type; };

	virtual void reset();

	virtual void run_instruction();
	virtual bool load_program(std::string file_path);
	virtual bool save_program_state(std::string& program_name, uint8_t state_number, uint32_t utc_timestamp);
	virtual void load_program_state(std::string file_name);
	virtual void print_current_opcode() { 
		printf("opcode: %04x\n", (static_cast<uint16_t>(memory[program_ctr]) << 8) + memory[program_ctr + 1]); 
	};

	virtual void process_key_event(uint8_t& key_index, bool is_pressed);
	virtual void cancel_key_wait_event();

	virtual void countdown_timers();

	inline static const time_t HZ_PER_SECOND = 60;
	uint16_t opcodes_per_second = 700;

	const std::string INTERPRETER_NAME;

	std::array<key_info, 16> keys = {};
	bool draw_flag = false;

	bool wait_for_key_release = false;

	inline static bool wait_for_display_update = true;

	bool is_paused = false;
	bool is_running = false;

	bool play_sfx = false;
	bool sound_disabled;

	// TODO: optimise how pixel states are stored by changing the above array to the following 
	// so that each bit is mapped to a pixel and each row of them is mapped to a 64bit variable
	// std::vector<uint8_t> px_states[(64 * 32) / 8] = {};
	std::vector<uint8_t> px_states;

	virtual uint8_t number_of_planes() { return px_states.size() / (native_height * native_width); };

protected:

	inline static const std::string SAVE_FILE_EXTENSION = ".sav";
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	// 0x200-0xFFF - Program ROM and work RAM

	inline static const uint32_t MEMORY_SIZE = 0x10000; //0x1000

	// most programs written for the original system begin at memory location
	// 0x200 because the interpreter occupied the first 512 bytes
	inline static const uint16_t PROGRAM_START_ADDR = 0x200;

	// stores an unmodified version of the application in the case that an opcode modifies it
	inline static uint8_t application_bytes[MEMORY_SIZE - PROGRAM_START_ADDR];

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack
	inline static size_t MAX_STACK_SIZE = 16;
	std::vector<uint16_t> stack;

	Chip8Type _type;
	uint8_t _selected_planes = 0b01;

	uint8_t memory[MEMORY_SIZE] = {};
	uint8_t registers[16] = {};

	uint16_t index_reg = 0;
	uint16_t program_ctr = PROGRAM_START_ADDR;

	// used to figure out if we should be saving data from memory into a save state
	uint16_t lowest_mem_addr_updated = 0xFFF;

	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;

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

	// OG key mapping    Recommend key mapping
	//   |1|2|3|C|            |1|2|3|4|
	//   |4|5|6|D|     =>     |Q|W|E|R|
	//   |7|8|9|E|            |A|S|D|F|
	//   |A|0|B|F|            |Z|X|C|V|

	// we create a SDL Scancodes instead of KeyCodes in the case that a non-QWERTY keyboard layout is used
	// since we want each key press to be mapped to a specific physical position on a keyboard
	static inline std::array<SDL_Scancode, 16> key_map = {
		SDL_SCANCODE_X, // 0
		SDL_SCANCODE_1, // 1
		SDL_SCANCODE_2, // 2
		SDL_SCANCODE_3, // 3
		SDL_SCANCODE_Q, // 4
		SDL_SCANCODE_W, // 5
		SDL_SCANCODE_E, // 6
		SDL_SCANCODE_A, // 7
		SDL_SCANCODE_S, // 8
		SDL_SCANCODE_D, // 9
		SDL_SCANCODE_Z, // A
		SDL_SCANCODE_C, // B
		SDL_SCANCODE_4, // C
		SDL_SCANCODE_R, // D
		SDL_SCANCODE_F, // E
		SDL_SCANCODE_V  // F
	};

	// Before the CHIP-8 interpreters CHIP48 and SUPER-CHIP (1970s - 1980s), the I register
	// was incremented each time it stored or loaded one register. (I += X + 1).
	bool increment_i = true;
	std::vector<uint8_t> additional_data;

	virtual bool run_additional_or_modified_instructions(uint16_t& opcode, uint8_t& VX_reg, uint8_t& VY_reg) { return false; };
	virtual void skip_instruction() { program_ctr += 2; };
	virtual void interpret_additional_data() = 0;
	virtual void update_gfx(uint8_t x, uint8_t y, uint8_t sprite_height) = 0;
};