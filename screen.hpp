#pragma once
#include <gtkmm/drawingarea.h>

class Screen : public Gtk::DrawingArea
{
public:
	Screen();
	virtual ~Screen();
	void init();

protected:
	void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

private:
	Gtk::Window* _parent_window;
	int _native_height = 400;
	int _native_width = 300;
};