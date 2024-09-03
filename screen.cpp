#include "screen.hpp"
#include <cairomm/context.h>
#include <gtkmm.h>

Screen::Screen()
{
	// using full size of window
	set_hexpand(true);
    set_halign(Gtk::Align::FILL);
    set_vexpand(true);
    set_valign(Gtk::Align::FILL);

	// using a set amount of the window
	// set_content_width(300);
	// set_content_height(400);

	set_draw_func(sigc::mem_fun(*this, &Screen::on_draw));
}

Screen::~Screen()
{
}

void Screen::init() 
{
	auto* root_widget = get_root();
	_parent_window = dynamic_cast<Gtk::Window*>(root_widget);
}

void Screen::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height)
{
	auto pixel_height = height / _native_height;
	auto pixel_width = width / _native_width;

	// coordinates for the center of the window
	int xc, yc;
	xc = width / 2;
	yc = height / 2;

	Gdk::RGBA bg_color;
    bg_color.set_rgba(0.0, 0.8, 0.9, 1.0);

	// Set the color in the Cairo context
    cr->set_source_rgba(bg_color.get_red(), bg_color.get_green(), bg_color.get_blue(), bg_color.get_alpha());
	cr->paint();

	// draw red lines out from the center of the window
	cr->set_line_width(1.0);
	cr->set_source_rgb(0.8, 0.6, 0.0);
	cr->move_to(0, 0);
	cr->line_to(xc, yc);
	cr->line_to(0, height);
	cr->move_to(xc, yc);
	cr->line_to(width, yc);
	cr->stroke();
}