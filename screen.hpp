#pragma once
#include <gtkmm/drawingarea.h>
#include "chip8.hpp"

class Screen : public Gtk::DrawingArea
{
public:
	Screen(Chip8* chip8_pointer);
	virtual ~Screen();

protected:
	void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

private:
	Gtk::Window* _parent_window;
	Chip8* _chip8_ptr;
};