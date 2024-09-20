#pragma once
#include "chip8.hpp"
#include <SDL2/SDL.h>

class Window;

class Screen
{
public:
	Screen(Chip8* chip8_pointer, Window& parent_window);
	virtual ~Screen();
	void generate_texture();
	void update_texture();
	SDL_Texture* get_texture() { return _texture; };
	SDL_FRect get_texture_dimensions();

private:
	Window& _parent_window;
	Chip8* _chip8_ptr;
	SDL_Texture* _texture = nullptr;
};