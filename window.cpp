#include "window.hpp"
//#include <gtk/gtk.h>
//#include <gtk/gtkmm.h>
#include <vector>
#include <string>
#include <memory>

Window::Window(GtkApplication* app, size_t width, size_t height)
{
	_window = gtk_application_window_new ( GTK_APPLICATION ( app ) );
	gtk_window_set_title ( GTK_WINDOW ( _window ), "Chip-8 Emulator" );
    gtk_window_set_default_size ( GTK_WINDOW ( _window ), width, height );
	// generate_menubar(app);

	add_menubar(app);
}

void Window::resize_window(size_t width, size_t height) {
	
}

static void action_clbk (GtkWidget *widget, gpointer data)
{
	g_print ("Hello World\n");
}

void Window::add_menubar(GtkApplication* app)
{
	generate_menubar(_window);
	gtk_application_set_menubar ( GTK_APPLICATION ( app ), G_MENU_MODEL ( menu ) );
	gtk_application_window_set_show_menubar ( GTK_APPLICATION_WINDOW ( _window ), TRUE );
}

void Window::generate_menubar(GtkApplication* app)
{
	/// *** Create Actions
	GSimpleAction *act_load_file = g_simple_action_new ( "load_file", NULL );
	GSimpleAction *act_save_state = g_simple_action_new ( "save_state", NULL );
	GSimpleAction *act_load_state = g_simple_action_new ( "load_state", NULL );
	GSimpleAction *act_scheme = g_simple_action_new ( "scheme", NULL );
	GSimpleAction *act_change_resolution = g_simple_action_new ( "resolution", NULL );
	GSimpleAction *act_quit = g_simple_action_new ( "quit", NULL );

	/// *** Add them to the ActionMap
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_load_file ) );
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_save_state ) );
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_load_state ) );
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_scheme ) );
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_change_resolution ) );
	g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( act_quit ) );

	/// *** Connect them to the activate Signal
	g_signal_connect ( act_load_file, "activate", G_CALLBACK ( action_clbk ), NULL );
	g_signal_connect ( act_save_state, "activate", G_CALLBACK ( action_clbk ), NULL );
	g_signal_connect ( act_load_state, "activate", G_CALLBACK ( action_clbk ), NULL );
	g_signal_connect ( act_scheme, "activate", G_CALLBACK ( action_clbk ), NULL );
	g_signal_connect ( act_change_resolution, "activate", G_CALLBACK ( action_clbk ), NULL );

	// Main bar that holds all menu dropdowns
	GMenu* menu = g_menu_new();

	// File menu items
	GMenu* menu_file = g_menu_new();
	g_menu_append_submenu ( menu, "File", G_MENU_MODEL ( menu_file ) );
	GMenuItem* menu_file_load = g_menu_item_new ( "Load File", "app.load_file" );
	g_menu_append_item(menu_file, menu_file_load);
	GMenuItem* menu_file_quit = g_menu_item_new ( "Quit", "app.quit" );
	g_menu_append_item(menu_file, menu_file_quit);

	GMenu* menu_states = g_menu_new();
	g_menu_append_submenu ( menu, "States", G_MENU_MODEL ( menu_states ) );

	GMenu* menu_states_save_menu = g_menu_new();
	GMenu* menu_states_load_menu = g_menu_new();

	for (uint8_t i = 0; i < MAX_SAVE_LOAD_STATES; i++) {
		GMenuItem* menu_states_save_item = g_menu_item_new ( std::string("Save State " + std::to_string(i)).c_str(), "app.save_state" );
		g_menu_append_item(menu_states_save_menu, menu_states_save_item);
		g_object_unref (menu_states_save_item);

		GMenuItem* menu_states_load_item = g_menu_item_new ( std::string("Load State " + std::to_string(i)).c_str(), "app.save_state" );
		g_menu_append_item(menu_states_load_menu, menu_states_load_item);
		g_object_unref (menu_states_load_item);
	}

	g_menu_append_submenu ( menu_states, "Save", G_MENU_MODEL ( menu_states_save_menu ) );
	g_menu_append_submenu ( menu_states, "Load", G_MENU_MODEL ( menu_states_load_menu ) );

	GMenu* menu_settings = g_menu_new();
	g_menu_append_submenu ( menu, "Settings", G_MENU_MODEL ( menu_settings ) );

	GMenu* menu_settings_resolution_menu = g_menu_new();

	for (uint8_t i = 1; i <= MAX_RESOLUTION_MAGNIFACTION; i++) {
		GMenuItem* menu_settings_resolution_item = g_menu_item_new ( std::string(std::to_string(i) + "x").c_str(), "app.change_resolution" );
		g_menu_append_item(menu_settings_resolution_menu, menu_settings_resolution_item);
		g_object_unref (menu_settings_resolution_item);
	}

	g_menu_append_submenu ( menu_settings, "Resolution", G_MENU_MODEL ( menu_settings_resolution_menu ) );

	GMenu* menu_settings_scheme = g_menu_new();
	g_menu_append_submenu ( menu_settings, "Color Scheme", G_MENU_MODEL ( menu_settings_scheme ) );

	gtk_application_set_menubar ( GTK_APPLICATION ( app ), G_MENU_MODEL ( menu ) );
	gtk_application_window_set_show_menubar ( GTK_APPLICATION_WINDOW ( _window ), TRUE );

	// Clean
	g_object_unref ( menu_file );
	g_object_unref ( menu_states_save_menu );
	g_object_unref ( menu_states_load_menu );
	g_object_unref ( menu_states );
	g_object_unref ( menu_settings_scheme );
	g_object_unref ( menu_settings );
	g_object_unref ( menu );
}

void Window::present (GtkApplication* app, gpointer user_data)
{
	gtk_window_present (GTK_WINDOW (_window));
}
