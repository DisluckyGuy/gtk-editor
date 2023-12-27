#include "Notes.hpp"

NotesApp::NotesApp() : Gtk::Application("github.DisluckyGuy.myNotes")
{
    Glib::set_application_name("MyNotes");
}

NotesApp::~NotesApp()
{
}

Glib::RefPtr<NotesApp> NotesApp::create()
{
    return Glib::RefPtr<NotesApp>(new NotesApp());
}

void NotesApp::on_window_hide(Gtk::Window *window)
{
    delete window;
}

void NotesApp::on_dark_theme()
{
  mainWindow->set_name("dark_window");
  mainWindow->set_theme(Theme::Dark);
}

void NotesApp::on_light_theme()
{
  mainWindow->set_name("light_window");
  mainWindow->set_theme(Theme::Light);
}

void NotesApp::on_file_save()
{
  mainWindow->writeToFile();
}

void NotesApp::on_file_save_as()
{
  mainWindow->on_file_save_as();
}

void NotesApp::on_open_file()
{
  mainWindow->on_open_file();
}

void NotesApp::on_new_file()
{
  mainWindow->on_new_file();
}

void NotesApp::create_window()
{
  auto provider = Gtk::CssProvider::create();

  provider->load_from_path("../windowStyle.css");

  

  mainWindow = new NotesWindow();

  auto ctx = mainWindow->get_style_context();
  ctx->add_class("theme");
  ctx->add_provider_for_display(mainWindow->get_display(), provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  //Make sure that the application runs for as long this window is still open:
  add_window(*mainWindow);
  mainWindow->set_name("dark_window");

  auto actionGroup = Gio::SimpleActionGroup::create();

  //Delete the window when it is hidden.
  //That's enough for this simple example.
  mainWindow->signal_hide().connect(sigc::bind(
    sigc::mem_fun(*this, &NotesApp::on_window_hide), mainWindow));
    mainWindow->set_show_menubar();
  mainWindow->show();
}

void NotesApp::on_startup()
{

  Gtk::Application::on_startup();
  add_action("quit", sigc::mem_fun(*this, &NotesApp::quit));
  add_action("darktheme",sigc::mem_fun(*this, &NotesApp::on_dark_theme));
  add_action("lighttheme", sigc::mem_fun(*this, &NotesApp::on_light_theme));
  add_action("save", sigc::mem_fun(*this, &NotesApp::on_file_save));
  add_action("saveas", sigc::mem_fun(*this, &NotesApp::on_file_save_as));
  add_action("openFile", sigc::mem_fun(*this, &NotesApp::on_open_file));
  add_action("new", sigc::mem_fun(*this, &NotesApp::on_new_file));
  set_accel_for_action("app.save", "<primary>s");
  set_accel_for_action("app.saveas", "<primary><shift>s");
  set_accel_for_action("app.new", "<primary>n");
  set_accel_for_action("app.openFile", "<primary>o");

  menuBuilder = Gtk::Builder::create();

  try
  {
    menuBuilder->add_from_file("../menu.ui");
  }
  catch (const Glib::Error& ex)
  {
    std::cerr << "Building menus failed: " << ex.what();
  }

  auto object = menuBuilder->get_object("menu-example");
  auto gmenu = std::dynamic_pointer_cast<Gio::Menu>(object);
  if (!gmenu)
  {
    g_warning("GMenu not found");
  }
  else
  {
    set_menubar(gmenu);
  }
  
}

void NotesApp::on_activate()
{
    create_window();
}


