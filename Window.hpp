#pragma once

#include <gtkmm.h>
#include <fstream>
#include <iostream>

enum class Theme{
    Dark,
    Light
};

struct textPage
{
    Gtk::ScrolledWindow scrollable;
    Glib::RefPtr<Gtk::TextBuffer> buffer;
    Gtk::TextView view;
    Gtk::Label label;
    std::string filePath = "";
    int pageNumber;
};


class NotesWindow : public Gtk::ApplicationWindow
{
    public:

    NotesWindow();
    ~NotesWindow();

    void set_theme(Theme theme);

    void setBufferText(std::string path, textPage* page);
    void writeToFile();
    void on_open_file();
    void on_new_file();
    void on_file_save_as();
    void on_close_tab();
    void on_window_open();
    void on_window_close();
    void on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);
    void on_new_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);
    void on_file_save_as_dialogue_response(int response_id, Gtk::FileChooserDialog* dialog);
    void on_no_file_opened_dialogue_response(int response_id,  Gtk::MessageDialog *dialog);
    void on_buffer_changed(textPage* page);

    private:

    Theme theme;
    Gtk::Grid mainGrid;
    Gtk::Frame notesFrame;
    Gtk::Button newNote;
    Gtk::Grid notesGrid;

    std::vector<textPage*> tabContents;
    Gtk::Notebook* textTabs;

    void initWindow();
    void initTabs();
    void initMainGrid();
    int getCurrentTextPage();
    void addPage(std::string path);
    void newPage(std::string path, std::string name);
    void newUntitled();
    void newPageWithName();
    void onNoFileOpened();
    void saveFileAs(std::string path, std::string name);
    void correctOrder();
};