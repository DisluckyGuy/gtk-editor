#include "Window.hpp"

NotesWindow::NotesWindow() : Gtk::ApplicationWindow()
{
    add_action("save", sigc::mem_fun(*this, &NotesWindow::writeToFile));
    initWindow();
    initTabs();
    initMainGrid();
    on_window_open();
}

NotesWindow::~NotesWindow()
{
    on_window_close();
}

void NotesWindow::set_theme(Theme theme)
{
    this->theme = theme;
    if (theme == Theme::Dark) {
        textTabs->set_name("dark_window"); 
        textTabs->get_first_child()->set_name("dark_window");
        textTabs->get_first_child()->set_name("dark_window");
        textTabs->get_last_child()->set_name("dark_window");
        for (textPage* page : tabContents) {
            page->view.set_name("dark_view"); 
            page->scrollable.set_name("dark_view"); 
        }
    } else if (theme == Theme::Light) {
        textTabs->set_name("light_window");
        textTabs->get_first_child()->set_name("light_window");
        textTabs->get_last_child()->set_name("light_window");
        textTabs->get_last_child()->set_name("light_window");
        for (textPage* page : tabContents) {
            page->view.set_name("light_view"); 
            page->scrollable.set_name("light_view"); 
        }
    }
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
        textTabs->set_tab_label(page->scrollable, page->label);
    } else {
        page->label.set_text(path);
        textTabs->set_tab_label(page->scrollable, page->label);
    }

    fb.close();
}

void NotesWindow::writeToFile()
{
    if (textTabs->get_n_pages() == 0) {
        onNoFileOpened();
        return;
    }
    if (tabContents[getCurrentTextPage()]->filePath == "") {
        on_file_save_as();
        return;
    }
    std::string name = tabContents[getCurrentTextPage()]->label.get_text();
    if (name[name.size() - 1] == '*') {
        name = name.substr(0, name.size() - 1);
        tabContents[getCurrentTextPage()]->label.set_text(name);
    }
    std::filebuf fb;
    std::string str = tabContents[getCurrentTextPage()]->buffer->get_text();
    fb.open(tabContents[getCurrentTextPage()]->filePath, std::ios::out);
    std::ostream os(&fb);
    os << str;
    fb.close();
    
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
    newUntitled();
}

void NotesWindow::on_file_save_as()
{
    if (textTabs->get_n_pages() == 0) {
        onNoFileOpened();
        return;
    }
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

void NotesWindow::on_close_tab()
{
    
    tabContents.erase(tabContents.begin() + getCurrentTextPage());
    textTabs->remove_page(textTabs->get_current_page() - 1);
    
    if (textTabs->get_n_pages() == 0) {
        get_application()->quit();
    }
}

void NotesWindow::on_window_open()
{
    
    std::filebuf fb;
    if (!fb.open("../configs/toOpen.config", std::ios::in)) {
        fb.open("../configs/toOpen.config", std::ios::out);
        fb.close();
    }
    std::string str;
    std::istream is(&fb);
    while (is) {
        getline(is, str);
        if (str == "") {
            break;
        }
        std::string name = str.substr(0,str.find_first_of(":"));
        std::string path = str.substr(str.find_first_of(":") + 2, str.size() - 1);
        
        addPage(path);
    }
    if (textTabs->get_n_pages() == 0) {
        newUntitled();
    }
    fb.close();
}

void NotesWindow::on_window_close()
{
    std::filebuf fb;
    
    if (!fb.open("../configs/toOpen.config", std::ios::out)) {
        std::cout << "couldn't open config file" << std::endl;
    }
    std::ostream os(&fb);

    for (textPage* page : tabContents) {
        std::string name = page->label.get_text();
        if (name[name.size() - 1] == '*') {
            name = name.substr(0, name.size() - 1);
        }
        if (name != "untitled") {
            os << name << ": " << page->filePath << std::endl;
        }
    }

    fb.close();
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

void NotesWindow::on_no_file_opened_dialogue_response(int response_id, Gtk::MessageDialog *dialog)
{
    switch (response_id)
    {
    case Gtk::ResponseType::OK: {
        dialog->close();
        break;
    }

    default:
        break;
    
    delete dialog;
    }
}

void NotesWindow::on_buffer_changed(textPage* page)
{
    if (!(page->label.get_text()[page->label.get_text().size() - 1] == '*')) {
        page->label.set_text(page->label.get_text() + "*");
    }
}

void NotesWindow::initWindow()
{
    set_title("MyNotes");
    set_default_size(600,500);
}

void NotesWindow::initTabs()
{

    textTabs = new Gtk::Notebook;
    textTabs->set_expand();
    textTabs->set_name("dark_tab");
    textTabs->get_first_child()->set_name("dark_tab");
    textTabs->get_last_child()->set_name("dark_tab");
    //textTabs->signal_page_reordered().connect(sigc::bind(sigc::mem_fun(*this, &NotesWindow::correctOrder), textTabs));
    
}

void NotesWindow::initMainGrid()
{
    mainGrid.attach(notesFrame, 0, 1, 5,6);
    mainGrid.attach(*textTabs,0,1,5,6);
    set_child(mainGrid);
}

int NotesWindow::getCurrentTextPage()
{
    correctOrder();
    for (int i = 0; i < tabContents.size();i++) {
        if (tabContents[i]->pageNumber == textTabs->get_current_page()) {
            std::cout << i << std::endl;
            return i;
        }
    }
    return 0;
}

void NotesWindow::addPage(std::string path)
{
    textPage* newPage = new textPage;

    textTabs->set_expand();
    
    
    

    tabContents.push_back(newPage);
    newPage->buffer = Gtk::TextBuffer::create();
    newPage->buffer->set_enable_undo();
    newPage->buffer->signal_changed().connect(sigc::bind(sigc::mem_fun(*this,  &NotesWindow::on_buffer_changed), newPage));

    newPage->view.set_buffer(newPage->buffer);
    newPage->view.set_margin(5); 

    newPage->scrollable.set_expand();
    newPage->scrollable.set_child(newPage->view);
    newPage->pageNumber = 0;
    if (theme == Theme::Dark) {
        textTabs->set_name("dark_tab");
        textTabs->get_first_child()->set_name("dark_tab");
        textTabs->get_last_child()->set_name("dark_tab");
        newPage->view.set_name("dark_view");
        newPage->scrollable.set_name("dark_view");
    } else if (theme == Theme::Light) {
        textTabs->set_name("light_tab");
        textTabs->get_first_child()->set_name("light_tab");
        textTabs->get_last_child()->set_name("light_tab");
        newPage->view.set_name("light_view");
        newPage->scrollable.set_name("light_view");
    }

    textTabs->append_page(newPage->scrollable, newPage->label);
    textTabs->set_tab_reorderable(newPage->scrollable);
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

    fb.close();
}

void NotesWindow::newUntitled()
{
    textPage* newPage = new textPage;

    textTabs->set_expand();

    tabContents.push_back(newPage);
    newPage->buffer = Gtk::TextBuffer::create();
    newPage->buffer->set_enable_undo();
    newPage->buffer->signal_changed().connect(sigc::bind(sigc::mem_fun(*this,  &NotesWindow::on_buffer_changed), newPage), true);
    

    newPage->view.set_buffer(newPage->buffer);

    newPage->view.set_margin(5);

    newPage->label.set_text("untitled");

    if (theme == Theme::Dark) {
        textTabs->set_name("dark_tab");
        textTabs->get_first_child()->set_name("dark_tab");
        textTabs->get_last_child()->set_name("dark_tab");
        newPage->view.set_name("dark_view");
        newPage->scrollable.set_name("dark_view");
    } else if (theme == Theme::Light) {
        textTabs->set_name("light_tab");
        textTabs->get_first_child()->set_name("light_tab");
        textTabs->get_last_child()->set_name("light_tab");
        newPage->view.set_name("light_view");
        newPage->scrollable.set_name("light_view");
    }

    

    newPage->scrollable.set_expand();
    newPage->scrollable.set_child(newPage->view);
    newPage->pageNumber = 0;

    textTabs->append_page(newPage->scrollable, newPage->label);
    textTabs->set_tab_reorderable(newPage->scrollable);
}

void NotesWindow::newPageWithName()
{
}

void NotesWindow::onNoFileOpened()
{
    auto dialog = new Gtk::MessageDialog("No File Error");
    dialog->set_transient_for(*this);
    dialog->set_modal();
    dialog->set_message("No file has been currently opened");
    //dialog->add_button("Close", Gtk::ResponseType::CLOSE);
    dialog->signal_response().connect(sigc::bind(
     sigc::mem_fun(*this, &NotesWindow::on_no_file_opened_dialogue_response), dialog));
    dialog->show();
}

void NotesWindow::saveFileAs(std::string path, std::string name)
{
    

    std::string labelName = tabContents[getCurrentTextPage()]->label.get_text();
    if (labelName[labelName.size() - 1] == '*') {
        labelName = labelName.substr(0, labelName.size() - 1);
        tabContents[getCurrentTextPage()]->label.set_text(labelName);
    }

    std::filebuf fb;
    if (!fb.open(path, std::ios::out)) {
        std::cout << "couldn't save to file path: " << path << std::endl;
    }

    std::string str = tabContents[getCurrentTextPage()]->buffer->get_text();
    std::ostream os(&fb);
    os << str;
    fb.close();

        if (tabContents[getCurrentTextPage()]->filePath == "") {
        if (textTabs->get_tab_label_text(tabContents[getCurrentTextPage()]->scrollable) == "untitled") {
            textTabs->set_tab_label_text(tabContents[getCurrentTextPage()]->scrollable, name);
        }
        tabContents[getCurrentTextPage()]->filePath = path;
        tabContents[getCurrentTextPage()]->buffer->signal_changed().connect(sigc::bind(sigc::mem_fun(*this,  &NotesWindow::on_buffer_changed), tabContents[getCurrentTextPage()]), true);
    } else {
        addPage(path);
    }
}

void NotesWindow::correctOrder()
{
    for (textPage* page : tabContents) {
        page->pageNumber = textTabs->page_num(page->scrollable); 
    }
}
