#include "window.hpp"

Window::Window()
	: m_box(Gtk::Orientation::VERTICAL)
{
	chip8 = Chip8();
	m_menubar.set_chip8_pointer(&chip8);

	set_title(default_title);

	m_box.append(m_menubar);
	m_box.append(screen);

	set_child(m_box);

	m_menubar.generate();
	m_menubar.set_size_request(-1, _native_menubar_height);

	set_default_size(Chip8::native_width, get_allocated_height() + Chip8::native_height + m_menubar.get_height());

	screen.init();
	
	printf("height: %d, width: %d\n", m_menubar.get_allocated_height(), m_menubar.get_width());

	auto controller = Gtk::EventControllerKey::create();

	controller->signal_key_pressed().connect(
  		sigc::mem_fun(*this, &Window::on_key_pressed), false
	);

	add_controller(controller);
}

void Window::main_loop() {
	if (chip8.draw_flag) {
		//screen.update_display(chip8.px_states, Chip::native_width, Chip8::native_height);
	}
}

int Window::get_native_width() {
	return _native_width;
}
int Window::get_native_height() {
	return _native_screen_height+_native_menubar_height;
}

bool Window::on_key_pressed(guint keyval, guint, Gdk::ModifierType state)
{
	// all valid keys are uppercase
	auto key_pressed = g_unichar_toupper(keyval);
	for (uint8_t i = 0; i < key_map.size(); i++) {

		if ((key_map[i] == key_pressed) &&
			// if ctrl or alt modifiers are used, mark key as un-pressed
			((state & (Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::ALT_MASK)) == 
				(state & NO_KEY_MODIFIER)
			)
		) {
			chip8.keys_pressed[i] = true;

			printf("Valid key press\n");
			return true;
		}
	}

	if (((state & ~(Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::LOCK_MASK)) == NO_KEY_MODIFIER) &&
		((state & (Gdk::ModifierType::CONTROL_MASK)) == Gdk::ModifierType::CONTROL_MASK)) {
		if (key_pressed == GDK_KEY_S) {

		} else if (key_pressed == GDK_KEY_L) {

		} else if (key_pressed == GDK_KEY_P) {

		}

		printf("Valid ctrl press\n");
		return true;
	}


	printf("char: %x, uppercase: %x, state: %d\n", keyval, g_unichar_toupper(keyval), static_cast<int>(state));

  	return false;
}