#include "Window.hpp"

NotesWindow::NotesWindow() : Gtk::ApplicationWindow()
{
    add_action("save", sigc::mem_fun(*this, &NotesWindow::writeToFile));
    initWindow();
    initTabs();
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
    } else {
        page->label.set_text(path);
        textTabs.set_tab_label(page->scrollable, page->label);
    }
}

void NotesWindow::writeToFile()
{

    if (tabContents[textTabs.get_current_page()]->filePath == "") {
        on_file_save_as();
    }
    std::filebuf fb;
    std::string str = tabContents[textTabs.get_current_page()]->buffer->get_text();
    fb.open(tabContents[textTabs.get_current_page()]->filePath, std::ios::out);
    std::ostream os(&fb);
    os << str;
    
}

void NotesWindow::on_open_file()
{
    auto dialog = new Gtk::FileChooserDialog("choose a file", Gtk::FileChooser::Action::OPEN);
    dialog->set_transient_for(*this);
    dialog->set_destroy_with_parent();
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    dialog->add_button("_Open", Gtk::ResponseType::OK);
    dialog->set_modal();
    dialog->signal_response().connect(sigc::bind(
    sigc::mem_fun(*this, &NotesWindow::on_file_dialog_response), dialog));
    dialog->show();
}

void NotesWindow::on_new_file()
{
    // auto dialog = new Gtk::MessageDialog();
    // dialog->set_transient_for(*this);
    // dialog->add_button("_Create", Gtk::ResponseType::OK);
    // dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    // dialog->set_modal();
    
    // //dialog->signal_response().connect(sigc::bind(sigc::mem_fun(*this, &NotesWindow::on_new_file_dialog_response), dialog));
    // dialog->show();
    newUntitled();

}

void NotesWindow::on_file_save_as()
{
     auto dialog = new Gtk::FileChooserDialog("choose a directory for your file", Gtk::FileChooser::Action::SAVE);
    dialog->set_transient_for(*this);
    dialog->set_destroy_with_parent();
    dialog->add_button("_Create", Gtk::ResponseType::OK);
    dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    dialog->set_modal();
    dialog->signal_response().connect(sigc::bind(
    sigc::mem_fun(*this, &NotesWindow::on_file_save_as_dialogue_response), dialog));
    dialog->set_transient_for(*this);
    dialog->show();
}

void NotesWindow::on_file_dialog_response(int response_id, Gtk::FileChooserDialog* dialog)
{
    switch (response_id)
    {
    case Gtk::ResponseType::CANCEL: {
        dialog->close();
        break;
    }
    
    case Gtk::ResponseType::OK: {
        std::string path = dialog->get_file()->get_path();
        addPage(path);
        dialog->close();
    }
    default:
        break;
    
    delete dialog;
    }
}

void NotesWindow::on_new_file_dialog_response(int response_id, Gtk::FileChooserDialog *dialog)
{
    switch (response_id)
    {
    case Gtk::ResponseType::CANCEL: {
        dialog->close();
        break;
    }
    
    case Gtk::ResponseType::OK: {
        std::string path = dialog->get_file()->get_path();
        std::string name = dialog->get_current_name();
        newPage(path, name);
        dialog->close();
    }
    default:
        break;
    
    delete dialog;
    }
}

void NotesWindow::on_file_save_as_dialogue_response(int response_id, Gtk::FileChooserDialog *dialog)
{
    switch (response_id)
    {
    case Gtk::ResponseType::CANCEL: {
        dialog->close();
        break;
    }
    
    case Gtk::ResponseType::OK: {
        std::string path = dialog->get_file()->get_path();
        std::string name = dialog->get_current_name();
        saveFileAs(path, name);
        dialog->close();
    }
    default:
        break;
    
    delete dialog;
    }
}

void NotesWindow::initWindow()
{
    set_title("MyNotes");
    set_default_size(600,500);
}

void NotesWindow::initTabs()
{

    textTabs.set_expand();
    textTabs.set_name("dark_tab");
    textTabs.get_first_child()->set_name("dark_tab");
    textTabs.get_last_child()->set_name("dark_tab");
    
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
    //textTabs.set_tab_reorderable(newPage->scrollable);
    newPage->filePath = path;
    setBufferText(newPage->filePath,newPage);
}

void NotesWindow::newPage(std::string path, std::string name)
{
    std::filebuf fb;

    if (!fb.open(path, std::ios::out)){
        std::cout << "couldn't create file in path " << path << std::endl; 
    }

    addPage(path);
}

void NotesWindow::newUntitled()
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

    newPage->label.set_text("untitled");

    newPage->scrollable.set_name("dark_view");
    newPage->scrollable.set_expand();
    newPage->scrollable.set_child(newPage->view);
    newPage->pageNumber = 0;

    textTabs.append_page(newPage->scrollable, newPage->label);
    //textTabs.set_tab_reorderable(newPage->scrollable);
}

void NotesWindow::newPageWithName()
{
}

void NotesWindow::saveFileAs(std::string path, std::string name)
{
    std::filebuf fb;
    if (!fb.open(path, std::ios::out)) {
        std::cout << "couldn't save to file path: " << path << std::endl;
    }
    
    if (tabContents[textTabs.get_current_page()]->filePath == "") {
        if (textTabs.get_tab_label_text(tabContents[textTabs.get_current_page()]->scrollable) == "untitled") {
            textTabs.set_tab_label_text(tabContents[textTabs.get_current_page()]->scrollable, name);
        }
        tabContents[textTabs.get_current_page()]->filePath = path;
    }
    std::string str = tabContents[textTabs.get_current_page()]->buffer->get_text();
    std::ostream os(&fb);
    os << str;
}
