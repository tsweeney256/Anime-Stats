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

#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include <wx/frame.h>
#include <memory>
#include "cppw/Sqlite3.hpp"
#include "Settings.hpp"

class DataPanel;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnPreferencesSortByPronunciation(wxCommandEvent& event);
    void OnDefaultDb(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);

    void SwitchToDataDir();
    void DoDefaultDbPopup();
    int SaveChangesPopup();
    //also handles creation of new DB if needed
    //will destroy the window the window if there is an unrecoverable error
    //else it will return nullptr if the error is recoverable
    //the only recoverable error should be if it fails to create a new db file
    std::unique_ptr<cppw::Sqlite3Connection> GetDbConnection(const wxString& file, bool eraseIfAlreadyExists = false);

    wxDECLARE_EVENT_TABLE();

    std::unique_ptr<cppw::Sqlite3Connection> m_connection;
    DataPanel* m_dataPanel = nullptr;
    std::unique_ptr<Settings> m_settings = nullptr;
    wxString m_dbFile;
    const char* const settingsFileName = "settings.xml";

    enum
    {
        SORT_BY_PRONUNCIATION = wxID_HIGHEST + 1,
        DEFAULT_DB
    };
};

#endif
