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
	if (_render_texture) SDL_DestroyTexture(_render_texture);
    if (_update_texture) SDL_DestroyTexture(_update_texture);
}

void Screen::generate_texture()
{
	_render_texture = SDL_CreateTexture(
		_parent_window.renderer_ptr,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		_chip8_ptr->native_width,
		_chip8_ptr->native_height
	);

	_update_texture = SDL_CreateTexture(
        _parent_window.renderer_ptr,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        _chip8_ptr->native_width,
        _chip8_ptr->native_height
    );

	if (!_render_texture || !_update_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create textures: %s", SDL_GetError());
    }
}

void Screen::swap_textures() {
    std::swap(_render_texture, _update_texture);
}

void Screen::update_texture()
{
	uint32_t* pixels;
	int pitch;

	if (SDL_LockTexture(_update_texture, NULL, (void**)&pixels, &pitch) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to lock texture: %s", SDL_GetError());
        return;
    }

	const auto& color_scheme = Window::COLOR_SCHEME_ARRAY[Window::selected_color_scheme];
	const auto& planes = _chip8_ptr->number_of_planes();
	uint16_t pixels_in_plane = _chip8_ptr->native_width * _chip8_ptr->native_height;

	// Directly modify the pixel data
	for (int i = 0; i < pixels_in_plane; i++) {

		// if a pixel in the 2nd plane is on
		if ((planes > 1) && _chip8_ptr->px_states[pixels_in_plane + i]) {
			pixels[i] = _chip8_ptr->px_states[i]
				? color_scheme.intersection_color
				: color_scheme.background_color;

		} else {
			pixels[i] = _chip8_ptr->px_states[i] 
				? color_scheme.foreground_color
				: color_scheme.unselected_plane_color;
		}
	}

	// Unlock the texture after modifying pixels
    SDL_UnlockTexture(_update_texture);
}

SDL_FRect Screen::get_texture_dimensions()
{
	float menu_bar_height = ImGui::GetFrameHeight();

	return {
		.x = 0,
		.y = menu_bar_height,
		.w = ImGui::GetMainViewport()->Size.x,
		.h = ImGui::GetMainViewport()->Size.y - menu_bar_height
	};
}