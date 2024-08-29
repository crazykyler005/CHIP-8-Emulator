#include "window.hpp"

Window::Window()
	: m_box(Gtk::Orientation::VERTICAL)
{
	set_title(default_title);
	set_default_size(_native_width, _native_menubar_height + _native_screen_height);

	m_box.append(m_menubar);
	m_box.append(screen);

	set_child(m_box);

	m_menubar.generate();
	m_menubar.set_size_request(-1, _native_menubar_height);

	screen.init();
	
	printf("height: %d, width: %d\n", m_menubar.get_allocated_height(), m_menubar.get_width());
}

int Window::get_native_width() {
	return _native_width;
}
int Window::get_native_height() {
	return _native_screen_height+_native_menubar_height;
}