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

#ifndef DATAPANEL_HPP
#define DATAPANEL_HPP

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <memory>
#include <vector>
#include "cppw/Sqlite3.hpp"
#include "AppIDs.hpp"
#include "SqlGridCommand.hpp"
#include "FilterStructs.hpp"

namespace cppw { class Sqlite3Connection; }
class MainFrame;
class wxMenu;
class Settings;

//the panel that holds all the components that will make up the Data page
class DataPanel : public wxPanel
{
public:
    DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top, Settings* settings, wxWindowID id = ID_NOTEBOOK,
              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
    bool UnsavedChangesExist();
    void SetUnsavedChanges(bool);
    void Undo();
    void Redo();
    void ClearCommandHistory();
    void ApplyFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
                     std::shared_ptr<AdvFilterInfo> newAdvFilterInfo, std::vector<wxString>* changedRows = nullptr);
    void SetAddedFilterRows(std::shared_ptr<std::vector<wxString>> changedRows);
    void NewFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
                   std::shared_ptr<AdvFilterInfo> newAdvFilterInfo);
    void SetSort(std::string  sqlSortStr);
    void SortByPronunciation(bool b);

    //This can't be done in the destructor for some reason or else it gets screwed up
    void WriteSizesToSettings();
    void SetSqlite3Connection(cppw::Sqlite3Connection* connection); //preserves everything on the panel and the command history
    void ResetPanel(cppw::Sqlite3Connection* connection); //resets everything except for color
    void RefreshFilter(); //does not refresh colors
    void RefreshGridColors(); //does not update value color mappings
    void UpdateCellColorInfo(); //update value color mappings
    wxArrayString GetColNames();
    const std::vector<wxString>* GetAllowedWatchedVals();
    const std::vector<wxString>* GetAllowedReleaseVals();
    const std::vector<wxString>* GetAllowedSeasonVals();
    MainFrame* GetTop() const;

private:
    void OnCheckAllBtn(wxCommandEvent& event);
    void OnUncheckAllBtn(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    void OnResetFilter(wxCommandEvent& event);
    void OnAdvFilter(wxCommandEvent& event);
    void OnAdvSort(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnAliasTitle(wxCommandEvent& event);
    void OnGridColSort(wxGridEvent& event);
    void OnGridCellChanging(wxGridEvent& event);
    void OnGridCellChanged(wxGridEvent& event);
    void OnAdvrFrameDestruction(wxWindowDestroyEvent& event);
    void OnLabelContextMenu(wxGridEvent& event);
    void OnLabelContextMenuItem(wxCommandEvent& event);

    void ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results);
    void AppendStatusStr(std::stringstream& statusStr, std::string toAppend, bool& firstStatus);
    void ApplyFullGrid();
    void NewBasicFilter();
    void HandleCommandChecking();
    void BuildAllowedValsMap(std::vector<wxString>& map, const std::string& sqlStmtStr);
    void HandleUndoRedoColorChange();
    void UpdateOldFilterData();
    std::string GetAddedRowsSqlStr(std::vector<wxString>* changedRows);
    void ResetFilterGui();
    void UpdateCellColor(int row, int col);
    void UpdateNumericalCellColorInfo(int col);
    int GetColAggregate(std::string colName, std::string function);
    int GetColMedian(const std::string& colName);
    void RefreshColColors(int col);
    void ShowSqliteBusyErrorBox();

    class CellColorInfo{
    public:
        int min = -1;
        int mid = -1;
        int max = -1;
        std::vector<wxString>* allowedVals = nullptr;
        enum{MIN, MID, MAX};
        enum{R, G, B};
    };

    MainFrame* m_top;
    wxGrid* m_grid;
    Settings* m_settings;
    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_blankCheck;
    wxCheckBox* m_toWatchCheck;
    wxButton* m_advFilterButton;
    wxButton* m_advSortButton;
    wxTextCtrl* m_titleFilterTextField;
    wxMenu* m_labelContextMenu = nullptr;
    wxString m_basicSelectString;
    cppw::Sqlite3Connection* m_connection;
    std::string m_curOrderCol = " Title collate nocase ";
    std::string m_curOrderDir = " asc ";
    std::string m_curOrderCombined = m_curOrderCol + m_curOrderDir;
    std::vector<std::unique_ptr<SqlGridCommand>> m_commands;
    std::vector<wxString> m_allowedWatchedVals;
    std::vector<wxString> m_allowedReleaseVals;
    std::vector<wxString> m_allowedSeasonVals;
    int m_commandLevel = 0;
    bool m_colsCreated = false;
    int m_curColSort = col::TITLE;
    bool m_curSortAsc = true;
    bool m_unsavedChanges = false;
    bool m_firstDraw = true;
    std::shared_ptr<BasicFilterInfo> m_basicFilterInfo;
    std::shared_ptr<AdvFilterInfo> m_advFilterInfo;
    std::shared_ptr<BasicFilterInfo> m_oldBasicFilterInfo;
    std::shared_ptr<AdvFilterInfo> m_oldAdvFilterInfo;
    wxBoxSizer* m_panelSizer;
    std::shared_ptr<std::vector<wxString>> m_changedRows; //managed by the command classes
    wxArrayString m_colList;
    CellColorInfo m_cellColorInfo[col::NUM_COLS];

    DECLARE_EVENT_TABLE()
};

#endif // DATAPANEL_HPP
