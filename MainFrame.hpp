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
class wxMenu;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint &pos, const wxSize& size);
    void SetUnsavedChanges(bool);
    bool UnsavedChangesExist();
    void UpdateStats();

private:
    void OnClose(wxCloseEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRows(wxCommandEvent& event);
    void OnAliasTitle(wxCommandEvent& event);
    void OnEditTags(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    void OnDefaultFilter(wxCommandEvent& event);
    void OnAdvFilter(wxCommandEvent& event);
    void OnAdvSort(wxCommandEvent& event);
    void OnMakeDefaultFilter(wxCommandEvent& event);
    void OnDefaultDbAsk(wxCommandEvent& event);
    void OnDefaultDb(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnColorOptions(wxCommandEvent& event);
    void OnImportMAL(wxCommandEvent& event);

    bool CreateMemoryDb();
    bool OpenDb(const wxString& file);
    void SwitchToDataDir();
    void DoDefaultDbPopup();
    int SaveChangesPopup();
    void NewOpenCommon(int style);
    void SetDbFlags(cppw::Sqlite3Connection* connection);
    bool WriteMemoryDbToFile();
    int GetDbVersion();
    bool UpdateDb(int version);
    void DbUpdateNotify();
    void UpdateNegOneDb();

    wxDECLARE_EVENT_TABLE();

    std::unique_ptr<cppw::Sqlite3Connection> m_connection;
    DataPanel* m_dataPanel = nullptr;
    wxMenu* m_fileMenu;
    wxMenu* m_preferencesMenu;
    std::unique_ptr<Settings> m_settings = nullptr;
    wxString m_dbFile;
    bool m_dbInMemory = false;
    const char* const settingsFileName = "settings.xml";
    bool m_unsavedChanges = false;

    enum
    {
        DEFAULT_DB = wxID_HIGHEST + 1,
        DEFAULT_DB_ASK,
        COLOR_OPTIONS,
        MAL_IMPORT,
        ADD_ROW,
        DELETE_ROWS,
        ALIAS_TITLE,
        EDIT_TAGS,
        APPLY_FILTER,
        DEFAULT_FILTER,
        ADV_FILTER,
        ADV_SORT,
        MAKE_DEFAULT_FILTER
    };
};

#endif
