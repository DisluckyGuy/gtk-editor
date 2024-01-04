#include "Notes.hpp"

int main(int argc, char **argv)
{
    auto app = NotesApp::create();
    return app->run(argc, argv);
}