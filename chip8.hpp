
#include <stdint.h>

class chip8 {

typedef struct __attribute__((__packed__)) uint12 { 
	unsigned val:12 = 0;
} uint12_t;

public:
	void reset();
	void initialize();
	void run_instruction();
	// uint8_t key_pressed();
	void draw(uint8_t& x, uint8_t& y, uint8_t pix_height);

	void load_program(char* file_name);
	void save_program_state(uint8_t state_number, uint32_t utc_timestamp);
	void load_program_state(std::string file_name);

private:
	std::string program_name = "";
	std::string SAVE_FILE_EXTENSION = ".sav";
	// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	// 0x200-0xFFF - Program ROM and work RAM

	inline static uint16_t MEMORY_SIZE = 4096; //0x1000

	inline static uint8_t font_addr_start = 0x000;
	inline static uint8_t font_addr_end = 0x1FF;

	inline static uint8_t call_stack_start = 0xEA0; // 0xEA0-0xEFF
	inline static uint8_t call_stack_end = 0xEFF;

	inline static uint8_t refresh_display_addr_start = 0xF00;
	inline static uint8_t refresh_display_addr_end = 0xFFF;

	// most programs written for the original system begin at memory location
	// 0x200 because the interpreter occupied the first 512 bytes
	inline static uint8_t program_start_addr = 0x200;

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

	uint8_t memory[MEMORY_SIZE] = {};
	uint8_t registers[16] = {};

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack

	// stack
	inline static MAX_STACK_SIZE = 16;
	std::vector<uint12_t> stack;
	// uint12_t stack[16] = {};

	// stack point may not be necessary if I use a vector size I can just 
	// reference the last program_ctr value added
	//uint8_t stack_ptr = 0;

	uint12_t index_reg = {};
	uint16_t opcode = 0;

	uint12_t program_ctr = { program_start_addr };

	uint8_t px_states[64 * 32] = {};

	// used to figure out if we should be saving data from memory into a save state
	uint16_t lowest_mem_addr_updated = 0xFFF;
};
