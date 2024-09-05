#include "window.hpp"

Window::Window()
	: m_box(Gtk::Orientation::VERTICAL), screen(&chip8)
{
	set_default_size(200, 200);
	chip8 = Chip8();
	m_menubar.set_chip8_pointer(&chip8);

	set_title(default_title);

	m_box.append(m_menubar);
	m_box.append(screen);

	set_child(m_box);

	m_menubar.generate();
	m_menubar.set_size_request(-1, _native_menubar_height);

	set_default_size(get_minimum_width(), get_minimum_height());

	auto controller = Gtk::EventControllerKey::create();

	controller->signal_key_pressed().connect(
  		sigc::mem_fun(*this, &Window::on_key_pressed), false
	);

	add_controller(controller);
}

void Window::main_loop()
{
	while (true) {
		chip8.run();

		if (chip8.is_paused) {
			return;
		}

		if (chip8.draw_flag) {
			screen.queue_draw();
			chip8.draw_flag = false;
		}

		if (chip8.play_sfx) {
			// play sound
			chip8.play_sfx = false;
		}

		g_usleep(Chip8::MICRO_SECONDS_PER_FRAME);
	}
}

int Window::get_minimum_width() {
	// int calculated_min_width;

	// while(calculated_min_width < MIN_WINDOW_WIDTH) {
	// 	calculated_min_width += chip8.native_width * 4;
	// }

	return chip8.native_width * 4;
}
int Window::get_minimum_height() {

	// int calculated_min_height = _native_menubar_height;

	// // TODO: get the actual size of the titlebar 
	int titlebar_height = 7;
	// calculated_min_height += titlebar_height;

	// while(calculated_min_height < MIN_WINDOW_HEIGHT) {
	// 	calculated_min_height += chip8.native_height;
	// }

	return (chip8.native_height * 4) + _native_menubar_height + titlebar_height;
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