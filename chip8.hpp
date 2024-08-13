
#include <stdint.h>

class chip8 {

typedef struct uint12 { 
	unsigned val:12 = 0;
} uint12_t;

public:
	void initialize();
	void run_instruction();
	// uint8_t key_pressed();
	void draw(uint8_t& x, uint8_t& y, uint8_t pix_height);

private:
	inline static uint8_t font_addr_start = 0x000;
	inline static uint8_t font_addr_start = 0x1FF;

	inline static uint8_t call_stack_start = 0xEA0; // 0xEA0-0xEFF
	inline static uint8_t call_stack_end = 0xEFF;

	inline static uint8_t refresh_display_addr_start = 0xF00;
	inline static uint8_t refresh_display_addr_end = 0xFFF;

	uint8_t memory[4096] = {};
	uint8_t registers[16] = {};

	// stack is used to remeber the current location before a jump operation is made
	// the program counter gets stored in the stack

	uint12_t stack[16] = {};
	uint16_t stack_ptr = 0;

	uint12_t index_reg = {};

	uint16_t opcode = 0;

	// most programs written for the original system begin at memory location
	// 0x200 because the interpreter occupied the first 512 bytes
	uint8_t program_ctr = 0x200;
};
