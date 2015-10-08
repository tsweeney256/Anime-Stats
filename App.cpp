#include "App.hpp"
#include "MainFrame.hpp"

IMPLEMENT_APP(App); //tells wxWidgets where our main class for our app is

bool App::OnInit()
{
    auto frame = new MainFrame("Anime Stats", wxPoint(50,50), wxSize(1024, 768));
    frame->Show(true);
    return true;
}
