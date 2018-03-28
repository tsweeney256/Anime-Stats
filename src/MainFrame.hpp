/*Anime Stats
  Copyright (C) 2015-2018 Thomas Sweeney
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

#include <memory>
#include <tuple>
#include <wx/frame.h>
#include "cppw/Sqlite3.hpp"
#include "Settings.hpp"
#include "AnalysisPanel.hpp"

class wxMenu;
class wxMenuItem;
class wxBookCtrlEvent;
class wxNotebook;
class DataPanel;
class TopBar;

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
    void OnGroupStats(wxCommandEvent& event);
    void OnMakeDefaultFilter(wxCommandEvent& event);
    void OnDefaultDbAsk(wxCommandEvent& event);
    void OnDefaultDb(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnColorOptions(wxCommandEvent& event);
    void OnImportMAL(wxCommandEvent& event);
    void OnTabChange(wxBookCtrlEvent& event);
    void OnViewDataTab(wxCommandEvent& event);
    void OnViewAnalysisTab(wxCommandEvent& event);

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
    void Reset(cppw::Sqlite3Connection* connection);
    void MakeTempSeriesTable();
    void EnableTabSpecificItems(
        const std::vector<std::tuple<int, int, wxMenu*, bool>>& rules);

    wxDECLARE_EVENT_TABLE();

    std::unique_ptr<cppw::Sqlite3Connection> m_connection;
    DataPanel* m_dataPanel = nullptr;
    AnalysisPanel* m_analysisPanel = nullptr;
    wxMenu* m_fileMenu;
    wxMenu* m_preferencesMenu;
    wxMenu* m_editMenu;
    wxMenu* m_filterMenu;
    std::unique_ptr<Settings> m_settings = nullptr;
    wxString m_dbFile;
    bool m_dbInMemory = false;
    const char* const settingsFileName = "settings.xml";
    bool m_unsavedChanges = false;
    TopBar* m_topBar = nullptr;
    wxNotebook* m_notebook = nullptr;
    bool m_onDataTab = true;
    bool m_firstRun = true;

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
        GROUP_STATS,
        MAKE_DEFAULT_FILTER,
        VIEW_DATA_TAB,
        VIEW_ANALYSIS_TAB
    };
};

#endif
