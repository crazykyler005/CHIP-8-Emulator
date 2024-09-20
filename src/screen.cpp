#include "screen.hpp"
#include "window.hpp"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "ImGuiFileDialog.h"
#include <SDL2/SDL.h>

Screen::Screen(Chip8* chip8_pointer, Window& parent_window)
	: _chip8_ptr(chip8_pointer), _parent_window(parent_window)
{
}

Screen::~Screen()
{
}

void Screen::generate_texture()
{
	uint32_t* pixels = new uint32_t[_chip8_ptr->native_width * _chip8_ptr->native_height];

	// Fill pixel buffer with data
	for (int i = 0; i < _chip8_ptr->native_width * _chip8_ptr->native_height; i++) {
		if (_chip8_ptr->px_states[i]) {
			pixels[i] = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32), 255, 0, 0, 255);  // Red color
		} else {
			pixels[i] = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32), 0, 0, 0, 255);  // Red color
		}
	}

	SDL_Texture* texture = SDL_CreateTexture(_parent_window.renderer_ptr, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, _chip8_ptr->native_width, _chip8_ptr->native_height);
	SDL_UpdateTexture(texture, NULL, pixels, _chip8_ptr->native_width * sizeof(uint32_t));

	_texture = texture;
}

SDL_FRect Screen::get_texture_dimensions()
{
	float menu_bar_height = ImGui::GetFrameHeight();

	SDL_FRect dstRect;
	dstRect.x = 0;        							// X position on the screen
	dstRect.y = menu_bar_height;        			// Y position on the screen
	dstRect.w = ImGui::GetMainViewport()->Size.x;   // Width of the texture
	dstRect.h = ImGui::GetMainViewport()->Size.y;   // Height of the texture

	return dstRect;
}