/*Anime Stats
  Copyright (C) 2018 Thomas Sweeney
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

#pragma once

#include <exception>
#include <wx/scrolwin.h>

namespace cppw
{
    class Sqlite3Connection;
}

class wxBoxSizer;
class MainFrame;
class TopBar;

class StatsPanelNotImplementedException : public std::exception
{
public:
    const char* what() const noexcept override;
};

class StatsPanel : public wxScrolledWindow
{
public:
    StatsPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
               cppw::Sqlite3Connection* connection, TopBar* topBar);
    virtual void ApplyFilter();
    virtual void DefaultFilter();
    virtual void AdvFilter();
    virtual void AdvSort();
    virtual void AddRow();
    virtual void DeleteRows();
    virtual void AliasTitle();
    virtual void EditTags();
protected:
    virtual void dummy() = 0;

    cppw::Sqlite3Connection* m_connection;
    wxBoxSizer* m_mainSizer;
    MainFrame* m_top;
};
