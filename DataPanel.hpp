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

//the panel that holds all the components that will make up the Data page
class DataPanel : public wxPanel
{
public:
    DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, wxWindow* top, wxWindowID id = ID_NOTEBOOK,
              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
    bool UnsavedChangesExist();
    void SetUnsavedChanges(bool);
    void Undo();
    void Redo();
    void ApplyFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
            std::shared_ptr<AdvFilterInfo> newAdvFilterInfo, std::vector<wxString>* changedRows = nullptr);
    void SetAddedFilterRows(std::shared_ptr<std::vector<wxString>> changedRows);
    void NewFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
                std::shared_ptr<AdvFilterInfo> newAdvFilterInfo);

private:
    void OnGeneralWatchedStatusCheckbox(wxCommandEvent& event);
    void OnEnableAllCheckbox(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    void OnResetFilter(wxCommandEvent& event);
    void OnAdvFilter(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnGridColSort(wxGridEvent& event);
    void OnGridCellChanging(wxGridEvent& event);
    void OnComboDropDown(wxCommandEvent& event);
    void OnAdvFilterFrameDestruction(wxWindowDestroyEvent& event);

    void ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results);
    void AppendStatusStr(std::stringstream& statusStr, std::string toAppend, bool& firstStatus);
    void ApplyFullGrid();
    void AppendLastGridRow(bool whiteOutPrevious);
    void NewBasicFilter();
    void HandleCommandChecking();
    void BuildAllowedValsMap(std::vector<wxString>& map, const std::string& sqlStmtStr);
    void SetRatingColor(int row, const char* valStr);
    void SetWatchedStatusColor(int row, const std::string& valStr);
    void HandleUndoRedoColorChange();
    void UpdateOldFilterData();
    std::string GetAddedRowsSqlStr(std::vector<wxString>* changedRows);

    class ratingColor{
    public:
        enum{MIN, MID, MAX};
        enum{R, G, B};
    };

    wxWindow* m_top;
    wxGrid* m_grid;
    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_blankCheck;
    wxCheckBox* m_allCheck;
    wxButton* m_advFilterButton;
    wxTextCtrl* m_titleFilterTextField;
    wxString m_basicSelectString;
    cppw::Sqlite3Connection* m_connection;
    std::string m_curOrderCol = "Title collate nocase";
    std::string m_curOrderDir = "asc";
    std::string m_oldCellComboIndex;
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
    bool m_ratingColorEnabled = true, m_watchedStatusColorEnabled = true;
    int m_ratingColor[3][3] {{248, 105, 107}, {255, 235, 132}, {99, 190, 123}};
    //the hex is bgr instead of rgb because wxwidgets implemented it backwards
    std::vector<wxColour> m_watchedStatusColor {0xFFFFFF, 0xFFC271, 0x94FF3B, 0x83B8F9, 0x7D7DFF};
    int m_minRating = 0;
    int m_midRating = 5;
    int m_maxRating = 9;
    wxBoxSizer* m_panelSizer;
    std::shared_ptr<std::vector<wxString>> m_changedRows; //managed by the command classes

    DECLARE_EVENT_TABLE()
};

#endif // DATAPANEL_HPP
