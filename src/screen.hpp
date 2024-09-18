#pragma once
#include "chip8.hpp"

class Screen
{
public:
	Screen(Chip8* chip8_pointer);
	virtual ~Screen();
	void render();

private:
	Chip8* _chip8_ptr;
};