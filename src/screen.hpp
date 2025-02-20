#pragma once
#include "chip8.hpp"
#include <SDL.h>
#include <memory>

class Window;

class Screen
{
public:
	Screen(std::shared_ptr<Chip8Interpreter> chip8_pointer, Window& parent_window);
	virtual ~Screen();
	void generate_texture();
	void update_texture();
	SDL_Texture* get_texture() { return _texture; };
	SDL_FRect get_texture_dimensions();

	void set_chip8_pointer(std::shared_ptr<Chip8Interpreter> chip8_pointer) { _chip8_ptr = chip8_pointer; };

private:
	Window& _parent_window;
	std::shared_ptr<Chip8Interpreter> _chip8_ptr;
	SDL_Texture* _texture = nullptr;
};