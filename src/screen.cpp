#include "screen.hpp"
#include "window.hpp"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "ImGuiFileDialog.h"
#include <SDL2/SDL.h>

Screen::Screen(std::shared_ptr<Chip8Interpreter> chip8_pointer, Window& parent_window)
	: _chip8_ptr(chip8_pointer), _parent_window(parent_window)
{

}


Screen::~Screen()
{
}

void Screen::generate_texture()
{
	SDL_Texture* texture = SDL_CreateTexture(
		_parent_window.renderer_ptr,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		_chip8_ptr->native_width,
		_chip8_ptr->native_height
	);

	_texture = texture;
}

void Screen::update_texture()
{
	uint32_t* pixels;
	int pitch;

	if (SDL_LockTexture(_texture, NULL, (void**)&pixels, &pitch) != 0) {
        return;
    }

	// Directly modify the pixel data
	for (int i = 0; i < _chip8_ptr->native_width * _chip8_ptr->native_height; i++) {
		if (_chip8_ptr->px_states[i]) {
			pixels[i] = Window::COLOR_SCHEME_ARRAY[Window::seletected_color_scheme].color1;
		} else {
			pixels[i] = Window::COLOR_SCHEME_ARRAY[Window::seletected_color_scheme].color2;
		}
	}

	// Unlock the texture after modifying pixels
    SDL_UnlockTexture(_texture);
}

SDL_FRect Screen::get_texture_dimensions()
{
	float menu_bar_height = ImGui::GetFrameHeight();

	SDL_FRect dstRect;
	dstRect.x = 0;
	dstRect.y = menu_bar_height;
	dstRect.w = ImGui::GetMainViewport()->Size.x;
	dstRect.h = ImGui::GetMainViewport()->Size.y - menu_bar_height;

	return dstRect;
}