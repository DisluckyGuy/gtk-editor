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
    std::string filePath;
    int *pageNumber;
};


class NotesWindow : public Gtk::ApplicationWindow
{
    public:

    NotesWindow();

    void set_theme(Theme theme);

    const Gtk::Notebook* getTextTabs() const;

    void setBufferText(std::string path, textPage* page);
    void writeToFile();

    private:

    Gtk::Grid mainGrid;
    Gtk::Frame notesFrame;
    Gtk::Button newNote;
    Gtk::Grid notesGrid;

    std::vector<textPage*> tabContents;
    Gtk::Notebook textTabs;

    void initWindow();
    void initTabs();
    void initNoteRects();
    void initMainGrid();
    void addPage(std::string path);
};