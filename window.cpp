#include "window.hpp"
#include <vector>
#include <string>
#include <memory>

Window::Window()
	: m_box(Gtk::Orientation::VERTICAL)
{
	set_title(default_title);
	set_default_size(400, 300);

	m_box.append(m_menubar);
	m_box.append(screen);

	set_child(m_box);

	m_menubar.generate();
}

int Window::get_native_width() {
	return _native_width;
}
int Window::get_native_height() {
	return _native_height;
}