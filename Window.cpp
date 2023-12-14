#include "Window.hpp"

NotesWindow::NotesWindow() : Gtk::ApplicationWindow()
{
    add_action("save", sigc::mem_fun(*this, &NotesWindow::writeToFile));
    initWindow();
    initTabs();
    ///initNoteRects();
    initMainGrid();
}

void NotesWindow::set_theme(Theme theme)
{
    if (theme == Theme::Dark) {
        textTabs.set_name("dark_window"); 
        textTabs.get_first_child()->set_name("dark_window");
        textTabs.get_first_child()->set_name("dark_window");
        textTabs.get_last_child()->set_name("dark_window");
        for (textPage* page : tabContents) {
            page->view.set_name("dark_view"); 
            page->scrollable.set_name("dark_view"); 
        }
    } else if (theme == Theme::Light) {
        textTabs.set_name("light_window");
        textTabs.get_first_child()->set_name("light_window");
        textTabs.get_last_child()->set_name("light_window");
        textTabs.get_last_child()->set_name("light_window");
        for (textPage* page : tabContents) {
            page->view.set_name("light_view"); 
            page->scrollable.set_name("light_view"); 
        }
    }
}

const Gtk::Notebook* NotesWindow::getTextTabs() const
{
    return &textTabs;
}

void NotesWindow::setBufferText(std::string path, textPage* page)
{
    std::filebuf fb;

    if (!fb.open(path, std::ios::in)) {
        std::cout << "couldn't open file" << path << std::endl;
    }
    
    std::istream is(&fb);
    while (is) {
        std::string temp;
        std::getline(is, temp);
        page->buffer->insert(page->buffer->end(),temp);
        page->buffer->insert(page->buffer->end(),"\n");
    }

    if (path.find_last_of("/") != std::string::npos) {
        page->label.set_text(path.substr(path.find_last_of("/") + 1, path[path.size()-1]));
        textTabs.set_tab_label(page->scrollable, page->label);
    }
}

void NotesWindow::writeToFile()
{

    
    std::filebuf fb;
    std::string str = tabContents[textTabs.get_current_page()]->buffer->get_text();
    fb.open(tabContents[textTabs.get_current_page()]->filePath, std::ios::out);
    std::ostream os(&fb);
    os << str;
    
}

void NotesWindow::initWindow()
{
    set_title("MyNotes");
    set_default_size(600,500);
}

void NotesWindow::initTabs()
{

    textPage* newPage = new textPage;

    textTabs.set_expand();
    textTabs.set_name("dark_tab");
    textTabs.get_first_child()->set_name("dark_tab");
    textTabs.get_last_child()->set_name("dark_tab");
    

    tabContents.push_back(newPage);
    for (textPage* page : tabContents) {
        page->buffer = Gtk::TextBuffer::create();
        page->buffer->set_enable_undo();
        page->view.set_buffer(page->buffer);

        page->view.set_name("dark_view"); 
        page->scrollable.set_name("dark_view");
        page->view.set_margin(5);
        
        page->scrollable.set_expand();
        page->scrollable.set_child(page->view);
        page->pageNumber = 0;
        textTabs.append_page(page->scrollable, page->label);
        textTabs.set_tab_reorderable(page->scrollable);
        page->filePath = "../text.txt";
        

    }
    setBufferText(tabContents[0]->filePath,tabContents[0]);
    addPage("../text2.txt");
}

void NotesWindow::initNoteRects()
{
    //newNote.set_label("+");
    //newNote.set_size_request(75,105);
    ////newNote.set_hexpand(false);
    //noteButtons.push_back(&newNote);
    //notesGrid.set_expand();
//
    ////notesGrid.set_orientation(Gtk::Orientation::VERTICAL);
    //notesGrid.set_expand();
    //notesGrid.set_margin(10);
    //notesGrid.set_row_spacing(10);
    //notesGrid.set_column_spacing(10);
    //for (Gtk::Button* button : noteButtons) {
    //    
    //    notesGrid.attach(*button,0,0,1,1);
    //}
//
    //notesFrame.set_child(notesGrid);
    //notesFrame.set_expand();
    //notesFrame.set_halign(Gtk::Align::FILL);
    //notesFrame.set_valign(Gtk::Align::FILL);
    //
//
    //
}

void NotesWindow::initMainGrid()
{
    mainGrid.attach(notesFrame, 0, 1, 5,6);
    mainGrid.attach(textTabs,0,1,5,6);
    set_child(mainGrid);
}

void NotesWindow::addPage(std::string path)
{
    textPage* newPage = new textPage;

    textTabs.set_expand();
    textTabs.set_name("dark_tab");
    textTabs.get_first_child()->set_name("dark_tab");
    textTabs.get_last_child()->set_name("dark_tab");
    

    tabContents.push_back(newPage);
    newPage->buffer = Gtk::TextBuffer::create();
    newPage->buffer->set_enable_undo();

    newPage->view.set_buffer(newPage->buffer);
    newPage->view.set_name("dark_view");
    newPage->view.set_margin(5); 

    newPage->scrollable.set_name("dark_view");
    newPage->scrollable.set_expand();
    newPage->scrollable.set_child(newPage->view);
    newPage->pageNumber = 0;

    textTabs.append_page(newPage->scrollable, newPage->label);
    textTabs.set_tab_reorderable(newPage->scrollable);
    newPage->filePath = path;
    setBufferText(newPage->filePath,newPage);
}