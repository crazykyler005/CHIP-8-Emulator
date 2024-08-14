
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

private:
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

	uint8_t memory[4096] = {};
	uint8_t registers[16] = {};

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack
	uint12_t stack[16] = {};
	uint8_t stack_ptr = 0;

	uint12_t index_reg = {};
	uint16_t opcode = 0;

	uint8_t program_ctr = program_start_addr;

	uint8_t px_states[64 * 32] = {};
};
