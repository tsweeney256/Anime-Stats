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
#include "SortStruct.hpp"
#include "StatsPanel.hpp"

namespace cppw { class Sqlite3Connection; }
class MainFrame;
class wxMenu;
struct Settings;
class DataGrid;
class QuickFilter;
class TopBar;

//the panel that holds all the components that will make up the Data page
class DataPanel : public StatsPanel
{
public:
    DataPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
              cppw::Sqlite3Connection* connection, Settings* settings,
              TopBar* topBar);
    void Undo();
    void Redo();
    void ApplyFilter() override;
    void DefaultFilter() override;
    void AddRow() override;
    void DeleteRows() override;
    void AliasTitle() override;
    void EditTags() override;
    void ClearCommandHistory();
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
    QuickFilter* GetQuickFilter();

private:
    void dummy() override {}

    void OnApplyFilter(wxCommandEvent& event);
    void OnDefaultFilter(wxCommandEvent& event);
    void OnEditTags(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnAliasTitle(wxCommandEvent& event);
    void OnGridColSort(wxGridEvent& event);
    void OnGridCellChanging(wxGridEvent& event);
    void OnGridCellChanged(wxGridEvent& event);
    void OnAdvrFrameDestruction(wxWindowDestroyEvent& event);
    void OnLabelContextMenu(wxGridEvent& event);
    void OnLabelContextMenuItem(wxCommandEvent& event);

    void ResetTable(cppw::Sqlite3Result* results);
    void HandleCommandChecking();
    void BuildAllowedValsMap(std::vector<wxString>& map, const std::string& sqlStmtStr);
    void HandleUndoRedoColorChange();
    void UpdateOldFilterData();
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

    DataGrid* m_grid;
    Settings* m_settings;
    wxTextCtrl* m_titleFilterTextField;
    wxMenu* m_labelContextMenu = nullptr;
    std::vector<std::unique_ptr<SqlGridCommand>> m_commands;
    std::vector<wxString> m_allowedWatchedVals;
    std::vector<wxString> m_allowedReleaseVals;
    std::vector<wxString> m_allowedSeasonVals;
    int m_commandLevel = 0;
    bool m_colsCreated = false;
    bool m_firstDraw = true;
    CellColorInfo m_cellColorInfo[col::NUM_COLS];
    QuickFilter* m_quickFilter;


    DECLARE_EVENT_TABLE()
};

#endif // DATAPANEL_HPP
