#pragma once


#include <gtkmm.h>
#include <vector>
#include <iostream>
#include "Window.hpp"

class NotesApp : public Gtk::Application
{

    public:
    
    NotesApp();
    ~NotesApp();

    static Glib::RefPtr<NotesApp> create();

    
    
    protected:

    void on_startup() override;
    void on_activate() override;

    private:

    void on_window_hide(Gtk::Window* window);

    void on_dark_theme();
    void on_light_theme();
    void on_file_save();
    void create_window();

    NotesWindow* mainWindow;

    Glib::RefPtr<Gtk::Builder> menuBuilder;



};