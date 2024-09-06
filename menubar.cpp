#include "menubar.hpp"
#include "helper_functions.hpp"
#include "window.hpp"

#include <cstdio>
#include <iostream>

MenuBar::MenuBar(Chip8* chip8_pointer, Window& parent_window)
	: _chip8_ptr(chip8_pointer), _parent_window(parent_window)
{
}

void MenuBar::generate() {
	auto* root_widget = get_root();
	// _parent_window = dynamic_cast<Gtk::Window*>(root_widget);

	// Create action group for window
	m_refActionGroup = Gio::SimpleActionGroup::create();

	// Add actions to the action group
	m_refActionGroup->add_action("load", sigc::mem_fun(*this, &MenuBar::on_menu_file_load));
	m_refActionGroup->add_action("quit", sigc::mem_fun(*this, &MenuBar::on_menu_file_quit));

	insert_action_group("app", m_refActionGroup);

	// Create a file menu model
	auto menumodel = Gio::Menu::create();
	auto submenu = Gio::Menu::create();
	submenu->append("_Load", "app.load");
	submenu->append("_Quit", "app.quit");
	menumodel->append_submenu("_File", submenu);

	// Create a state menu model
	auto state_submenu = Gio::Menu::create();

	m_refActionGroup->add_action("pause", sigc::mem_fun(*this, &MenuBar::on_menu_state_pause));
	state_submenu->append("_Pause", "app.pause");

	auto save_state_submenu = Gio::Menu::create();

	for (int i = 0; i < 4; i++) {
		auto action_name = "save" + std::to_string(i);
		
		auto save_item = Gio::MenuItem::create("_Save State " + std::to_string(i), "app." + action_name);
		save_state_submenu->append_item(save_item);

		m_refActionGroup->add_action(action_name, [this, i, save_item]() { 
			this->on_menu_state_save(i);
		});
	}

	state_submenu->append_submenu("_Save States", save_state_submenu);

	auto load_state_submenu = Gio::Menu::create();

	for (int i = 0; i < 4; i++) {
		auto action_name = "load" + std::to_string(i);

		m_refActionGroup->add_action(action_name, [this, i]() { 
			this->on_menu_state_load(i);
		});

		load_state_submenu->append("_Load State " + std::to_string(i), "app." + action_name);
	}

	state_submenu->append_submenu("_Load States", load_state_submenu);
	menumodel->append_submenu("_States", state_submenu);

	auto settings_submenu = Gio::Menu::create();

	// generating color scheme submenu to settings menu
	auto resolution_submenu = Gio::Menu::create();
	for (int i = 4; i <= 16; i+=4) {
		auto action_name = "x" + std::to_string(i);

		m_refActionGroup->add_action(action_name, [this, i]() { 
			this->on_menu_update_resolution(i);
		});

		resolution_submenu->append("_" + action_name, "app." + action_name);
	}
	settings_submenu->append_submenu("_Resolution", resolution_submenu);

	// generating color scheme submenu to settings menu
	auto scheme_submenu = Gio::Menu::create();
	for (int i = 0; i < 4; i++) {
		auto action_name = "scheme" + std::to_string(i);

		m_refActionGroup->add_action(action_name, [this, i]() { 
			this->on_menu_update_resolution(i);
		});

		scheme_submenu->append("_Test " + std::to_string(i), "app." + action_name);
	}
	settings_submenu->append_submenu("_Scheme", scheme_submenu);

	menumodel->append_submenu("_Settings", settings_submenu);

	// Set up the menu bar
	set_menu_model(menumodel);
}

// https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-dialogs-filedialog.html
// The FileChooserDialog (deprecated since gtkmm 4.10)
void MenuBar::on_menu_file_load()
{
	std::cout << "File -> Load selected" << std::endl;

	auto dialog = Gtk::FileDialog::create();

	// Add filters, so that only certain file types can be selected:
	auto filters = Gio::ListStore<Gtk::FileFilter>::create();

	auto filter_ch8 = Gtk::FileFilter::create();
	filter_ch8->set_name("Chip-8 Files");
	filter_ch8->add_pattern("*.ch8");
	filters->append(filter_ch8);

	auto filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	filters->append(filter_any);

	dialog->set_filters(filters);

	// Show the dialog and wait for a user response:
	dialog->open(sigc::bind(sigc::mem_fun(
		*this, &MenuBar::on_file_dialog_finish), dialog)
	);
}

void MenuBar::on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
  const Glib::RefPtr<Gtk::FileDialog>& dialog)
{
  // Handle the response:
	try
	{
		auto file = dialog->open_finish(result);

		// Notice that this is a std::string, not a Glib::ustring.
		auto filename = file->get_basename();
		auto file_path = file->get_path();
		std::cout << "File path: " <<  file_path << std::endl;

		if (!_chip8_ptr->load_program(file_path)) {
			std::cout << "Falied to load file. " << std::endl;
		}

		_parent_window.set_title(Chip8::DEFAULT_TITLE + " - " + filename);
		_parent_window.main_loop();

	}
	catch (const Gtk::DialogError& err)
	{
		// Can be thrown by dialog->open_finish(result).
		std::cout << "No file selected. " << err.what() << std::endl;
	}
	catch (const Glib::Error& err)
	{
		std::cout << "Unexpected exception. " << err.what() << std::endl;
	}
}

void MenuBar::on_menu_file_quit()
{
	_parent_window.close();
}

void MenuBar::on_menu_state_save(int i)
{
	std::cout << "States -> Save file " + std::to_string(i) << std::endl;

	// need to pause the program here, generate a save state, then resume the application
	auto utc_timestamp = utc_time_in_seconds();
	_chip8_ptr->save_program_state(i, utc_timestamp);
	// auto state_idx = static_cast<uint8_t>(i);
	// auto utc = utc_time_in_seconds();

	// save program state into file
	// chip8.save_program_state(state_idx, utc);

	// save_menu_items[i]->set_label("test");

	// TODO: figure out how to update the label of the item after it's been added to the menu
	// menu_item->set_label("Save State " + std::to_string(state_idx) + " - " + get_time_str(true, utc)).c_str());
}

void MenuBar::on_menu_state_load(int i)
{
	// _chip8_ptr->load_program_state(load_menu_items[i]->get_label());
	std::cout << "States -> Load file " + std::to_string(i) << std::endl;
}

void MenuBar::on_menu_state_pause()
{
	if (_chip8_ptr->is_paused) {
		_chip8_ptr->is_paused = false;
		_parent_window.main_loop();

		return;
	}

	_chip8_ptr->is_paused = true;
	std::cout << "States -> Pause Program";
}

void MenuBar::on_menu_update_resolution(int i)
{
	auto current_screen_height = _parent_window.get_child()->get_height() - _height;
	auto current_titlebar_height = _parent_window.get_height() - (current_screen_height + _height);
	auto updated_screen_height = Chip8::native_height * i;

	// TODO: figure out why this doesn't update the window dimensions if the same resolution setting is selected more than once
	_parent_window.set_default_size(Chip8::native_width * i, updated_screen_height + _height + current_titlebar_height);
	printf("test\n");
}
