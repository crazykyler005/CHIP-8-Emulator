#include "screen.hpp"
#include <cairomm/context.h>
#include <gtkmm.h>

Screen::Screen(Chip8* chip8_pointer)
{
	_chip8_ptr = chip8_pointer;

	// using full size of window
	set_hexpand(true);
	set_halign(Gtk::Align::FILL);
	set_vexpand(true);
	set_valign(Gtk::Align::FILL);

	set_draw_func(sigc::mem_fun(*this, &Screen::on_draw));
}

Screen::~Screen()
{
}

void Screen::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
	double pixel_height = static_cast<double>(height) / static_cast<double>(_chip8_ptr->native_height);
	double pixel_width = static_cast<double>(width) / static_cast<double>(_chip8_ptr->native_width);

	Gdk::RGBA bg_color;
	bg_color.set_rgba(0.0, 0.8, 0.9, 1.0);

	// Set the color in the Cairo context
	cr->set_source_rgba(bg_color.get_red(), bg_color.get_green(), bg_color.get_blue(), bg_color.get_alpha());
	cr->paint();

	cr->set_source_rgb(0.8, 0.6, 0.0);

	for (uint8_t i = 0; i < _chip8_ptr->native_height; i++) {
		for (uint8_t j = 0; j < _chip8_ptr->native_width; j++) {
			if (_chip8_ptr->px_states[(i * _chip8_ptr->native_width) + j] == 1) {
				cr->rectangle(j * pixel_width, i * pixel_height, pixel_width, pixel_height);
			}
		}
	}

	cr->fill();
}