/*Anime Stats
Copyright (C) 2016 Thomas Sweeney
This file is part of Anime Stats.
Anime Stats is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
Anime Stats is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "App.hpp"
#include "MainFrame.hpp"

IMPLEMENT_APP(App); //tells wxWidgets where our main class for our app is

bool App::OnInit()
{
    auto frame = new MainFrame("Anime Stats", wxPoint(50,50), wxSize(1024, 768));
    frame->Show(true);
    return true;
}
