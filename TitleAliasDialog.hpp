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

#ifndef TITLEALIASDIALOG_HPP
#define TITLEALIASDIALOG_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <wx/dialog.h>

namespace cppw { class Sqlite3Connection; }
class wxListCtrl;
class wxListEvent;
namespace SqlTitleAliasCommand { class SqlTitleAliasCommand; }

//Originally I wanted to put a wxMenu on here to allow undoing and redoing actions. The code supporting undoing
//and redoing is even already there in SqlTitleAliasCommand. However, wxDialog does not support
//having menus. This makes it so wxDialogs can not support undoing and redoing without having to tinker with how
//wxWidgets handles keyboard events. The choice became having a wxFrame that supports menus but does not support
//being modal, or having a wxDialog that does not support menus but does support being modal. I chose making the window modal.

class TitleAliasDialog : public wxDialog
{
public:
    TitleAliasDialog() = delete;
    TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries, wxString title);

private:
    void OnActivated(wxListEvent& event);
    void OnBeginUpdate(wxListEvent& event);
    void OnEndUpdate(wxListEvent& event);
    void OnDelete(wxListEvent& event);
    void OnOk(wxCommandEvent& event);

    cppw::Sqlite3Connection* m_connection;
    int64_t m_idSeries;
    wxString m_title;
    wxListCtrl* m_list = nullptr;
    std::vector<std::unique_ptr<SqlTitleAliasCommand::SqlTitleAliasCommand>> m_commands;
    wxString m_oldEditVal;
    bool m_ignoreDeleteEvent = false;;

    DECLARE_EVENT_TABLE()
};

#endif
