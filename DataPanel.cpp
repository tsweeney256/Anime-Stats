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

#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/debug.h>
#include <wx/msgdlg.h>
#include <wx/combobox.h>
#include <wx/menu.h>
#include <wx/utils.h>
#include <wx/dcclient.h>
#include <wx/stdpaths.h>
#include <wx/dialog.h>
#include "DataPanel.hpp"
#include "AppIDs.hpp"
#include "cppw/Sqlite3.hpp"
#include "CustomGridCellEditors.hpp"
#include "TitleAliasDialog.hpp"
#include "EditTagDialog.hpp"
#include "MainFrame.hpp"
#include "Settings.hpp"
#include "SqlStrings.hpp"
#include "Helpers.hpp"
#include "QuickFilter.hpp"
#include "TopBar.hpp"

BEGIN_EVENT_TABLE(DataPanel, wxPanel)
    EVT_COMMAND(wxID_ANY, QuickFilterProcessEnterEvent, DataPanel::OnTextEnter)
    EVT_COMMAND(TopBar::id_apply_filter_btn, TopBarButtonEvent,
                DataPanel::OnApplyFilter)
    EVT_COMMAND(TopBar::id_default_filter_btn, TopBarButtonEvent,
                    DataPanel::OnDefaultFilter)
    EVT_COMMAND(TopBar::id_add_row_btn, TopBarButtonEvent,
                    DataPanel::OnAddRow)
    EVT_COMMAND(TopBar::id_delete_row_btn, TopBarButtonEvent,
                    DataPanel::OnDeleteRow)
    EVT_COMMAND(TopBar::id_alias_title_btn, TopBarButtonEvent,
                DataPanel::OnAliasTitle)
    EVT_COMMAND(TopBar::id_edit_tags_btn, TopBarButtonEvent,
                DataPanel::OnEditTags)
    EVT_GRID_COL_SORT(DataPanel::OnGridColSort)
    EVT_GRID_CELL_CHANGING(DataPanel::OnGridCellChanging)
    EVT_GRID_CELL_CHANGED(DataPanel::OnGridCellChanged)
    EVT_GRID_LABEL_RIGHT_CLICK(DataPanel::OnLabelContextMenu)
    EVT_MENU_RANGE(ID_VIEW_COL_BEGIN, ID_VIEW_COL_END, DataPanel::OnLabelContextMenuItem)
END_EVENT_TABLE()

DataPanel::DataPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
                     cppw::Sqlite3Connection* connection, Settings* settings,
                     TopBar* topBar)
    : StatsPanel(parent, top, id, connection), m_settings(settings),
      m_quickFilter(topBar->GetQuickFilter())
{
    ////
    ////grid
    ////

    m_grid = new wxGrid(this, ID_DATA_GRID);
    m_grid->CreateGrid(0,0);

    readFileIntoString(m_basicSelectString, "basicSelect.sql", top);
    BuildAllowedValsMap(m_allowedWatchedVals, "select status from WatchedStatus order by idWatchedStatus");
    BuildAllowedValsMap(m_allowedReleaseVals, "select type from ReleaseType order by idReleaseType");
    BuildAllowedValsMap(m_allowedSeasonVals, "select season from Season order by idSeason");
    //
    //panel sizer
    //
    m_panelSizer = new wxBoxSizer(wxVERTICAL);
    m_panelSizer->Add(topBar, wxSizerFlags(0).Border(wxALL, 2));
    m_panelSizer->Add(m_grid, wxSizerFlags(1).Expand().Border(wxALL, 0));
    SetSizerAndFit(m_panelSizer);

    //
    //Misc initializations
    //
    ResetPanel(m_connection);
}

void DataPanel::Undo()
{
    try{
        if(m_commandLevel > 0){
            m_commands[--m_commandLevel]->UnExecute();
            HandleUndoRedoColorChange();
        }
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error while undoing action.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::Redo()
{
    try{
        if(m_commandLevel <= static_cast<int>(m_commands.size()) - 1){
            m_commands[m_commandLevel++]->Execute();
            HandleUndoRedoColorChange();
        }
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error while redoing action.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::AddRow()
{
    m_grid->GoToCell(m_grid->GetNumberRows()-1, col::TITLE);
    m_grid->EnableCellEditControl(true);
}

void DataPanel::DeleteRows()
{
    auto rows = m_grid->GetSelectedRows();
    if (rows.GetCount() > 0) {
        std::vector<int64_t> idSeries;
        for(unsigned int i = 0; i < rows.GetCount(); ++i){
            auto idSeriesStr = m_grid->GetCellValue(rows.Item(i), col::ID_SERIES);
            if(idSeriesStr.compare("")) //ignore the last row
                idSeries.push_back(strtoll(idSeriesStr, nullptr, 10));
        }
        try{
            m_commands.push_back(std::make_unique<DeleteCommand>(m_connection, m_grid, idSeries));

        }catch(cppw::Sqlite3Exception& e){
            if(e.GetErrorCode() == SQLITE_BUSY){
                ShowSqliteBusyErrorBox();
                return;
            }
            else{
                wxMessageBox(std::string("Error deleting row(s)\n") + e.what());
                m_top->Close(true);
                return;
            }
        }
        ++m_commandLevel;
        HandleCommandChecking();
        m_top->SetUnsavedChanges(true);
    }
}

void DataPanel::AliasTitle()
{
    auto rows = m_grid->GetSelectedRows();

    if(rows.size() > 1)
        wxMessageBox("Error: You may only set aliases to one title at a time.");
    else if(rows.size() == 0)
        wxMessageBox("Error: No row was selected.");
    else if(rows[0] == m_grid->GetNumberRows()-1) {
        wxMessageBox("Error: Invalid row");
    }
    else{
        TitleAliasDialog aliasDlg(this, wxID_ANY, m_connection,
                                             wxAtol(m_grid->GetCellValue(rows[0], col::ID_SERIES)), m_grid->GetCellValue(rows[0], col::TITLE));
        if(aliasDlg.ShowModal() == wxID_OK)
            m_top->SetUnsavedChanges(true);
    }
}

void DataPanel::EditTags()
{
    auto rows = m_grid->GetSelectedRows();

    if(rows.size() > 1)
        wxMessageBox("Error: You may only tag one series at a time.");
    else if(rows.size() == 0)
        wxMessageBox("Error: No row was selected.");
    else if(rows[0] == m_grid->GetNumberRows()-1) {
        wxMessageBox("Error: Invalid row");
    }
    else{
        EditTagDialog editTagDlg(
            this, wxID_ANY, m_connection,
            wxAtol(m_grid->GetCellValue(rows[0], col::ID_SERIES)),
            m_grid->GetCellValue(rows[0], col::TITLE));
        editTagDlg.ShowModal();
        if (editTagDlg.MadeChanges()) {
            m_top->SetUnsavedChanges(true);
        }
    }
}

void DataPanel::DefaultFilter()
{
    m_quickFilter->LoadDefaultFilter();
    ApplyFilter();
}

void DataPanel::ClearCommandHistory()
{
    m_commands.clear();
    m_commandLevel = 0;
}

void DataPanel::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

void DataPanel::OnApplyFilter(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

void DataPanel::OnDefaultFilter(wxCommandEvent& WXUNUSED(event))
{
    DefaultFilter();
}

void DataPanel::OnAddRow(wxCommandEvent& WXUNUSED(event))
{
    AddRow();
}

void DataPanel::OnDeleteRow(wxCommandEvent& WXUNUSED(event))
{
    DeleteRows();
}

void DataPanel::OnAliasTitle(wxCommandEvent& WXUNUSED(event))
{
    AliasTitle();
}

void DataPanel::OnEditTags(wxCommandEvent& WXUNUSED(event))
{
    EditTags();
}

void DataPanel::OnGridColSort(wxGridEvent& event)
{
    int col = event.GetCol();
    std::string name;
    if(col == col::TITLE) {
        name = "nameSort";
    } else {
        name = std::string(m_grid->GetColLabelValue(col).utf8_str());
    }
    auto sortingRules = *m_quickFilter->GetSort();
    if (sortingRules.size() == 1 && sortingRules[0].name == name) {
        sortingRules[0].asc = !sortingRules[0].asc;
    } else {
        sortingRules.clear();
        sortingRules.emplace_back(name, true);
    }
    m_quickFilter->SetSort(sortingRules);
    ApplyFilter();
}

void DataPanel::OnGridCellChanging(wxGridEvent& event)
{
    //because wxGrid will select the first column upon presisng C-home
    //even if it is hidden, because that is a very rational design choice.
    //catching EVT_GRID_SELECT_CELL doesnt work for some reason ever after
    //forcing it to select the TITLE cell. it just selects the ID_SERIES
    //cell on the next row upon trying to edit the newly selected cell
    if (event.GetCol() == col::ID_SERIES) {
        event.Veto();
        return;
    }
    bool successfulEdit = true;

    //if adding a new entry
    if(event.GetRow() == m_grid->GetNumberRows()-1 && event.GetCol() == col::TITLE){
        try{
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_grid, this, std::string(event.GetString().utf8_str()), 1));
            AppendWriteProtectedRow(m_grid, col::TITLE+1, numViewCols, true);

        }catch(cppw::Sqlite3Exception& e){
            if(e.GetErrorCode() == SQLITE_BUSY){
                ShowSqliteBusyErrorBox();
                event.Veto();
                return;
            }
            else{
                wxMessageBox(std::string("Error making InsertCommand.\n") + e.what());
                m_top->Close(true);
            }
        }catch(SqlGridCommandException& e){
            wxMessageBox("Error: " + std::string(e.what()));
            successfulEdit = false;
            event.Veto();
            return;
        }
    }
    else{ //if updating value
        try{
            auto idSeries = strtoll(m_grid->GetCellValue(event.GetRow(), col::ID_SERIES).ToUTF8(), nullptr, 10);
            std::string newVal;
            std::string oldVal;
            auto col = event.GetCol();
            std::vector<wxString>* map = nullptr;
            if(event.GetCol() == col::RELEASE_TYPE || event.GetCol() == col::SEASON ||
               event.GetCol() == col::WATCHED_STATUS){
                if(event.GetCol() == col::RELEASE_TYPE)
                    map = &m_allowedReleaseVals;
                else if(event.GetCol() == col::SEASON)
                    map = &m_allowedSeasonVals;
                else if(event.GetCol() == col::WATCHED_STATUS){
                    map = &m_allowedWatchedVals;
                }
                newVal = std::to_string(
                    std::find(map->begin(), map->end(), event.GetString().utf8_str()) - map->begin());
                oldVal = std::to_string(
                    std::find(std::begin(*map), std::end(*map),
                              m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str()) - map->begin());
            }
            else{
                newVal = std::string(event.GetString().utf8_str());
                oldVal = std::string(m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str());
            }

            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_grid, this, idSeries, newVal, oldVal, col,
                                                                 map, 1));

        }catch(cppw::Sqlite3Exception& e){
            if(e.GetErrorCode() == SQLITE_BUSY){
                ShowSqliteBusyErrorBox();
                event.Veto();
                return;
            }
            else{
                wxMessageBox(std::string("Error making UpdateCommand.\n") + e.what());
                m_top->Close(true);
            }
        }catch(SqlGridCommandException& e){
            wxMessageBox("Error: " + std::string(e.what()));
            successfulEdit = false;
            event.Veto();
            return;
        }
    }
    if(successfulEdit){
        ++m_commandLevel;
        HandleCommandChecking();
        m_top->SetUnsavedChanges(true);
    }
}

void DataPanel::OnGridCellChanged(wxGridEvent& event)
{
    //this isn't even remotely optimized, but it runs so fast anyway I'm not sure it needs to be
    if(col::isColNumeric(event.GetCol())){
        UpdateNumericalCellColorInfo(event.GetCol());
        RefreshColColors(event.GetCol());
    }
    else{
        UpdateCellColor(event.GetRow(), event.GetCol());
    }
    if(col::isColLimitedValue(event.GetCol())){
        //Because wxGridCellChoiceEditor::BeginEdit() makes an undocumented call to Combo->SetFocus()
        //and never gives back the focus, like every other control, because wxWidgets is a well designed library.
        m_grid->SetFocus();
    }
    m_grid->Refresh(); //needed for Windows
}

void DataPanel::OnLabelContextMenu(wxGridEvent& event)
{
    if(event.GetRow() < 0)
        m_grid->PopupMenu(m_labelContextMenu, event.GetPosition());
    else
        event.Skip();
}

void DataPanel::OnLabelContextMenuItem(wxCommandEvent& event)
{
    int col = event.GetId() - ID_VIEW_COL_BEGIN;
    if(event.IsChecked()){
        m_grid->ShowCol(col);
        //ShowCol() automatically restores the previous column size, but if the previous column size was 0
        //we're going to have it autosize the column instead of having ShowCol() set it to its smaller default size
        if(!m_settings->colSizes[col - col::FIRST_VISIBLE_COL])
            m_grid->AutoSizeColumn(col, false);
    }
    else
        m_grid->HideCol(col);
}

void DataPanel::ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results)
{
    wxGridUpdateLocker lock(m_grid);
    int rowSize = 1; //don't initialize to 0, because a rowSize of 0 hides the row

    if(m_grid->GetNumberRows() > 0){
        rowSize = m_grid->GetRowSize(0);
        m_grid->DeleteRows(0, m_grid->GetNumberRows());
    }
    if(!m_colsCreated){
        m_colsCreated = true;
        m_grid->AppendCols(numViewCols);
        auto watchedAttr = new wxGridCellAttr();
        auto releasedAttr = new wxGridCellAttr();
        auto seasonAttr = new wxGridCellAttr();
        auto intAttr = new wxGridCellAttr();
        auto dateAttr = new wxGridCellAttr();
        watchedAttr->SetEditor(new wxGridCellChoiceEditor(m_allowedWatchedVals.size(), &m_allowedWatchedVals[0]));
        releasedAttr->SetEditor(new wxGridCellChoiceEditor(m_allowedReleaseVals.size(), &m_allowedReleaseVals[0]));
        seasonAttr->SetEditor(new wxGridCellChoiceEditor(m_allowedSeasonVals.size(), &m_allowedSeasonVals[0]));
        auto intEditor = new CustomGridCellNumberEditor(6); //only allow 6 length as an easy way to prevent integer overflows
        intAttr->SetEditor(intEditor);
        dateAttr->SetEditor(new GridCellDateEditor());
        m_grid->SetColAttr(col::RATING, intAttr);
        m_grid->SetColAttr(col::WATCHED_STATUS, watchedAttr);
        m_grid->SetColAttr(col::RELEASE_TYPE, releasedAttr);
        m_grid->SetColAttr(col::YEAR, intAttr);
        m_grid->SetColAttr(col::SEASON, seasonAttr);
        m_grid->SetColAttr(col::EPISODES_WATCHED, intAttr);
        m_grid->SetColAttr(col::TOTAL_EPISODES, intAttr);
        m_grid->SetColAttr(col::REWATCHED_EPISODES, intAttr);
        m_grid->SetColAttr(col::EPISODE_LENGTH, intAttr);
        m_grid->SetColAttr(col::DATE_STARTED, dateAttr);
        m_grid->SetColAttr(col::DATE_FINISHED, dateAttr);
        //increment the intAttr reference counter by how many columns use it minus 1
        for(int i = 0; i < 5; ++i)
            intAttr->IncRef();
        dateAttr->IncRef(); //an extra time for the date finished column
        m_labelContextMenu = new wxMenu();
        for(int i = 0; i < numViewCols; ++i){
            auto colName = wxString::FromUTF8(results->GetColumnName(i).c_str());
            m_grid->SetColLabelValue(i, colName);
            if(i != col::ID_SERIES){
                int id = ID_VIEW_COL_BEGIN + i;
                m_labelContextMenu->Append(id, colName, "", wxITEM_CHECK);
                m_labelContextMenu->Check(id, true);
            }
        }
        UpdateCellColorInfo();
    }
    int rowPos = 0;
    while(results->NextRow()){
        m_grid->AppendRows();
        m_grid->SetRowSize(rowPos, rowSize);
        m_grid->DisableRowResize(rowPos);
        for(int i = 0; i < numViewCols; ++i){
            m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
            UpdateCellColor(rowPos, i);
        }
        ++rowPos;
    }
    //user shouldn't see idSeries key
    m_grid->HideCol(col::ID_SERIES);
    AppendWriteProtectedRow(m_grid, col::TITLE+1, numViewCols, false);
    if(m_firstDraw){
        m_firstDraw = false;
        if(m_settings->colSizes.size()){
            for(size_t i = 0; i < m_settings->colSizes.size(); ++i){
                m_grid->SetColSize(col::FIRST_VISIBLE_COL + i, m_settings->colSizes[i]);
                if(m_settings->colSizes[i] == 0)
                    m_labelContextMenu->Check(ID_VIEW_COL_BEGIN + col::FIRST_VISIBLE_COL + i, false);
            }
        }
        else{
            m_grid->AutoSize();
            //set the Title column to be sized differently
            wxClientDC dc(m_grid);
            dc.SetFont(m_grid->GetLabelFont());
            auto labelTextSize(dc.GetTextExtent(m_grid->GetColLabelValue(col::TITLE)));
            m_grid->SetColSize(col::TITLE, labelTextSize.x * 10);
            //store sizes in the settings structure
            for(size_t i = 0; i < col::NUM_COLS - col::FIRST_VISIBLE_COL; ++i){
                m_settings->colSizes.emplace_back(m_grid->GetColSize(col::FIRST_VISIBLE_COL + i));
            }
            //pronunciation should be hidden by default. Most people don't need it.
            m_settings->colSizes[col::PRONUNCIATION - col::FIRST_VISIBLE_COL] = 0;
            m_grid->SetColSize(col::PRONUNCIATION, 0);
            m_labelContextMenu->Check(ID_VIEW_COL_BEGIN + col::PRONUNCIATION, false);
        }
    }
}

void DataPanel::ApplyFilter()
{
    try{
        //setting up the where part of the sql statement to filter by watched statuses
        ConstFilter filter = m_quickFilter->GetFilter();
        auto newBasicFilterInfo = filter.first;
        auto newAdvFilterInfo = filter.second;
        std::string watchedStatus = "idWatchedStatus";
        std::string releaseType = "idReleaseType";
        std::string season = "idSeason";
        bool firstStatus = true;
        bool showNothing = false; //if none of the boxes are ticked then nothing should be displayed
        std::stringstream statusStr;

        //watchedStatus
        if(newBasicFilterInfo->watched)
            AppendStatusStr(statusStr, watchedStatus + "= 1 ", firstStatus);
        if(newBasicFilterInfo->watching)
            AppendStatusStr(statusStr, watchedStatus + "= 2 ", firstStatus);
        if(newBasicFilterInfo->stalled)
            AppendStatusStr(statusStr, watchedStatus + "= 3 ", firstStatus);
        if(newBasicFilterInfo->dropped)
            AppendStatusStr(statusStr, watchedStatus + "= 4 ", firstStatus);
        if(newBasicFilterInfo->toWatch)
            AppendStatusStr(statusStr, watchedStatus + "= 5 ", firstStatus);
        if(newBasicFilterInfo->watchedBlank)
            AppendStatusStr(statusStr, watchedStatus + "= 0 ", firstStatus);
        if(!firstStatus)
            statusStr << " ) ";
        else
            showNothing = true;

        bool usingTagKey = false;
        bool usingTagVal = false;
        if(newAdvFilterInfo){
	    firstStatus = true;

            firstStatus = true;
            if(newAdvFilterInfo->tv)
                AppendStatusStr(statusStr, releaseType + "= 1 ", firstStatus);
            if(newAdvFilterInfo->ova)
                AppendStatusStr(statusStr, releaseType + "= 2 ", firstStatus);
            if(newAdvFilterInfo->ona)
                AppendStatusStr(statusStr, releaseType + "= 3 ", firstStatus);
            if(newAdvFilterInfo->movie)
                AppendStatusStr(statusStr, releaseType + "= 4 ", firstStatus);
            if(newAdvFilterInfo->tvSpecial)
                AppendStatusStr(statusStr, releaseType + "= 5 ", firstStatus);
            if(newAdvFilterInfo->releaseBlank)
                AppendStatusStr(statusStr, releaseType + "= 0 ", firstStatus);
            if(!firstStatus)
                statusStr << " ) ";
            else
                showNothing = true;

            firstStatus = true;
            if(newAdvFilterInfo->winter)
                AppendStatusStr(statusStr, season + "= 1 ", firstStatus);
            if(newAdvFilterInfo->spring)
                AppendStatusStr(statusStr, season + "= 2 ", firstStatus);
            if(newAdvFilterInfo->summer)
                AppendStatusStr(statusStr, season + "= 3 ", firstStatus);
            if(newAdvFilterInfo->fall)
                AppendStatusStr(statusStr, season + "= 4 ", firstStatus);
            if(newAdvFilterInfo->seasonBlank)
                AppendStatusStr(statusStr, season + "= 0 ", firstStatus);
            if(!firstStatus)
                statusStr << " ) ";
            else
                showNothing = true;

            if(newAdvFilterInfo->ratingEnabled)
                statusStr << " and (rating between " << newAdvFilterInfo->ratingLow << " and " <<
                    newAdvFilterInfo->ratingHigh << ") ";
            if(newAdvFilterInfo->yearEnabled)
                statusStr << " and (year between " << newAdvFilterInfo->yearLow << " and "
                          << newAdvFilterInfo->yearHigh << ") ";
            if(newAdvFilterInfo->epsWatchedEnabled)
                statusStr << " and (episodesWatched between " << newAdvFilterInfo->epsWatchedLow <<
                    " and " << newAdvFilterInfo->epsWatchedHigh <<") ";
            if(newAdvFilterInfo->totalEpsEnabled)
                statusStr << " and (totalEpisodes between " << newAdvFilterInfo->totalEpsLow <<
                    " and " << newAdvFilterInfo->totalEpsHigh << ") ";
            if(newAdvFilterInfo->epsRewatchedEnabled)
                statusStr << " and (rewatchedEpisodes between " << newAdvFilterInfo->epsRewatchedLow <<
                    " and " << newAdvFilterInfo->epsRewatchedHigh << ") ";
            if(newAdvFilterInfo->lengthEnabled)
                statusStr << " and (episodeLength between " << newAdvFilterInfo->lengthLow << " and " <<
                    newAdvFilterInfo->lengthHigh << ") ";
            if(newAdvFilterInfo->dateStartedEnabled){
                statusStr << " and (dateStarted between date('";
                statusStr << std::setfill('0') << std::setw(4) << newAdvFilterInfo->yearStartedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->monthStartedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->dayStartedLow;
                statusStr << "') and date('";
                statusStr << std::setfill('0') << std::setw(4) << newAdvFilterInfo->yearStartedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->monthStartedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->dayStartedHigh;
                statusStr << "')) ";
            }
            if(newAdvFilterInfo->dateFinishedEnabled){
                statusStr << " and (dateFinished between date('";
                statusStr << std::setfill('0') << std::setw(4) << newAdvFilterInfo->yearFinishedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->monthFinishedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->dayFinishedLow;
                statusStr << "') and date('";
                statusStr << std::setfill('0') << std::setw(4) << newAdvFilterInfo->yearFinishedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->monthFinishedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) << newAdvFilterInfo->dayFinishedHigh;
                statusStr << "')) ";
            }
            if(newAdvFilterInfo->tagKeyEnabled &&
               !newAdvFilterInfo->tagKeyInverse) {
                usingTagKey = true;
                statusStr << " and tag like ?2 ";
            }
            if(newAdvFilterInfo->tagValEnabled &&
               !newAdvFilterInfo->tagValInverse) {
                usingTagVal = true;
                statusStr << " and val like ?3 ";
            }
        }
        auto sqlStr = std::string(m_basicSelectString.utf8_str()) +
            " where 1=1 " + //just a dumb hack so I don't have to worry about when to start using 'and's and 'or's
            (showNothing ? " and 1 <> 1 " : statusStr.str());
        if (newAdvFilterInfo->tagKeyInverse || newAdvFilterInfo->tagValInverse) {
            sqlStr += "\nexcept\n" + sqlStr;
        }
        if(newAdvFilterInfo->tagKeyEnabled &&
           newAdvFilterInfo->tagKeyInverse) {
            usingTagKey = true;
            sqlStr += " and tag like ?2 ";
        }
        if(newAdvFilterInfo->tagValEnabled &&
           newAdvFilterInfo->tagValInverse) {
            usingTagVal = true;
            sqlStr += " and val like ?3 ";
        }
        sqlStr += " order by " + CreateSortStr();
        auto statement = m_connection->PrepareStatement(sqlStr);
        std::string bindStr = "%" + newBasicFilterInfo->title + "%";
        statement->Bind(1, bindStr);
        if (usingTagKey) {
            statement->Bind(2, newAdvFilterInfo->tagKey);
        }
        if (usingTagVal) {
            statement->Bind(3, newAdvFilterInfo->tagVal);
        }
        auto results = statement->GetResults();
        ResetTable(results);
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error applying filter.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::AppendStatusStr(std::stringstream& statusStr, std::string toAppend, bool& firstStatus)
{
    if(!firstStatus)
        statusStr << " or ";
    else{
        statusStr << " and ( ";
        firstStatus = false;
    }
    statusStr << toAppend;
}

std::string DataPanel::CreateSortStr()
{
    const auto& sortingRules = *m_quickFilter->GetSort();
    std::string sortStr;
    for(size_t i = 0; i < sortingRules.size(); ++i) {
        sortStr += "`" + sortingRules[i].name + "` collate nocase " +
            (sortingRules[i].asc ? "asc " : "desc ") +
            (i + 1 == sortingRules.size() ? "" : ", ");
    }
    return sortStr;
}

void DataPanel::HandleCommandChecking()
{
    if(m_commandLevel != static_cast<int>(m_commands.size())){
        m_commands.erase(m_commands.begin()+m_commandLevel-1, m_commands.end()-1);
    }
}

void DataPanel::BuildAllowedValsMap(std::vector<wxString>& map, const std::string& sqlStmtStr)
{
    //assumes the primary keys are in order and contiguous from 0 to n and that the value to be mapped
    //is the first column
    try{
        auto stmt = m_connection->PrepareStatement(sqlStmtStr);
        auto results = stmt->GetResults();
        while(results->NextRow())
            map.emplace_back(results->GetString(0).c_str(), wxMBConvUTF8());

    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error building allowed values map.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::HandleUndoRedoColorChange()
{
    auto rows = m_grid->GetSelectedRows();
    if(rows.Count()){
        UpdateCellColorInfo();
        for(unsigned int i = 0; i < rows.Count(); ++i){
            for(int k=col::FIRST_VISIBLE_COL; k < col::NUM_COLS; ++k){
                UpdateCellColor(rows.Item(i), k);
            }
        }
    }
    auto cells = m_grid->GetSelectionBlockTopLeft();
    if(cells.Count() == 1){ //there should never be more than one cell selected from undoing/redoing
        int row = cells.Item(0).GetRow();
        for (int i = col::FIRST_VISIBLE_COL; i < col::NUM_COLS; ++i){
            if(col::isColNumeric(i)){
                UpdateNumericalCellColorInfo(i);
                RefreshColColors(i);

            }else{
                UpdateCellColor(row, i);
            }
        }
    }
    m_grid->Refresh();
}

void DataPanel::WriteSizesToSettings()
{
    for(int i = 0; i < col::NUM_COLS - col::FIRST_VISIBLE_COL; ++i){
        m_settings->colSizes[i] = m_grid->GetColSize(col::FIRST_VISIBLE_COL + i);
    }
}

void DataPanel::SetSqlite3Connection(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
    for(auto& command : m_commands){
        command->SetSqlite3Connection(connection);
    }
}

void DataPanel::ResetPanel(cppw::Sqlite3Connection* connection)
{
    m_commands = std::vector<std::unique_ptr<SqlGridCommand>>();
    m_commandLevel = 0;
    m_connection = connection;
    UpdateCellColorInfo();
    DefaultFilter();
}

wxArrayString DataPanel::GetColNames()
{
    wxArrayString arr;
    for(int i = col::FIRST_VISIBLE_COL; i < col::NUM_COLS; ++i){
        arr.Add(m_grid->GetColLabelValue(i));
    }
    return arr;
}

const std::vector<wxString>* DataPanel::GetAllowedWatchedVals()
{
    return &m_allowedWatchedVals;
}

const std::vector<wxString>* DataPanel::GetAllowedReleaseVals()
{
    return &m_allowedReleaseVals;
}

const std::vector<wxString>* DataPanel::GetAllowedSeasonVals()
{
    return &m_allowedSeasonVals;
}

MainFrame* DataPanel::GetTop() const
{
    return m_top;
}

void DataPanel::UpdateCellColor(int row, int col)
{
    if(col != col::ID_SERIES){
        int offsetCol = col - col::FIRST_VISIBLE_COL;
        bool setBgColor = false;
        bool valColorEnabled = m_settings->cellColors[offsetCol][Settings::VAL] > -1;
        const auto& cellVal = m_grid->GetCellValue(row, col);

        if(m_settings->cellColors[offsetCol][Settings::TEXT] > -1){
            m_grid->SetCellTextColour(row, col, wxColour(m_settings->cellColors[offsetCol][Settings::TEXT]));
        }
        else{
            m_grid->SetCellTextColour(row, col, m_grid->GetDefaultCellTextColour());
        }

        if(!cellVal.empty() && valColorEnabled){
            if(col::isColLimitedValue(col)){
                wxASSERT(m_cellColorInfo[col].allowedVals);
                int idx = -1;
                //can't get the index of the combobox control because they possibly haven't been created yet and it'd be way
                //more inefficient to pre-create a combobox for every cell than to just do a few string comparisons for each one
                for(size_t i = 0; i < m_cellColorInfo[col].allowedVals->size(); ++i)
                    if(!m_grid->GetCellValue(row, col).compare((*m_cellColorInfo[col].allowedVals)[i])){
                        //account for blanks being tracked by allowedVals but not in Settings
                        idx = i - (Settings::VAL - Settings::BACKGROUND);
                        break;
                    }
                wxASSERT_MSG(idx > -1, "Illegal Limited Value");
                //blank vals handled by the set background color
                if(idx != 0 - (Settings::VAL - Settings::BACKGROUND)){
                    m_grid->SetCellBackgroundColour(row, col, wxColour(m_settings->cellColors[offsetCol][Settings::VAL + idx]));
                    setBgColor = true;
                }
            }else if(col::isColNumeric(col)){
                long val;
                if(!cellVal.ToLong(&val)){
                    wxMessageBox("Error converting cell to long while coloring");
                    m_top->Close(true);
                }
                long extremeVal;
                int extremeColorIdx;
                long midVal = m_cellColorInfo[col].mid;
                if(val > midVal){
                    extremeVal = m_cellColorInfo[col].max;
                    extremeColorIdx = CellColorInfo::MAX;
                }else{
                    extremeVal = m_cellColorInfo[col].min;
                    extremeColorIdx = CellColorInfo::MIN;
                }
                int componentColors[3][3];
                for(int i = Settings::MIN; i < Settings::MAX + 1; ++i){
                    wxColour color(m_settings->cellColors[offsetCol][i]);
                    componentColors[i - Settings::MIN][CellColorInfo::R] =  color.Red();
                    componentColors[i - Settings::MIN][CellColorInfo::G] =  color.Green();
                    componentColors[i - Settings::MIN][CellColorInfo::B] =  color.Blue();
                }
                int cellColour[3];
                for(int i=0; i<3; ++i){
                    int valNumerator = std::abs(val - midVal);
                    int valDenominator = std::abs(midVal - extremeVal);
                    int colorMultiplier = componentColors[extremeColorIdx][i] - componentColors[CellColorInfo::MID][i];
                    if(valDenominator == 0){
                        cellColour[i] = componentColors[CellColorInfo::MID][i];
                    }
                    else{
                        cellColour[i] = valNumerator/static_cast<double>(valDenominator) * colorMultiplier + componentColors[CellColorInfo::MID][i];
                    }
                }
                m_grid->SetCellBackgroundColour(row, col,
                                                wxColour(cellColour[CellColorInfo::R], cellColour[CellColorInfo::G], cellColour[CellColorInfo::B]));
                setBgColor = true;
            }
        }
        if(!setBgColor && m_settings->cellColors[offsetCol][Settings::BACKGROUND] > -1){
            m_grid->SetCellBackgroundColour(row, col, wxColour(m_settings->cellColors[offsetCol][Settings::BACKGROUND]));
        }
        else if(!setBgColor){
            m_grid->SetCellBackgroundColour(row, col, m_grid->GetDefaultCellBackgroundColour());
        }
    }
}

void DataPanel::UpdateCellColorInfo()
{
    for(int i = 0; i < col::NUM_COLS; ++i){
        m_cellColorInfo[i].allowedVals = nullptr;
    }
    m_cellColorInfo[col::WATCHED_STATUS].allowedVals = &m_allowedWatchedVals;
    m_cellColorInfo[col::RELEASE_TYPE].allowedVals = &m_allowedReleaseVals;
    m_cellColorInfo[col::SEASON].allowedVals = &m_allowedSeasonVals;

    for(auto col : numericCols){
        UpdateNumericalCellColorInfo(col);
    }
}

void DataPanel::UpdateNumericalCellColorInfo(int col)
{
    m_cellColorInfo[col].min = GetColAggregate(colViewName[col], "min");
    m_cellColorInfo[col].mid = GetColMedian(colViewName[col]);
    m_cellColorInfo[col].max = GetColAggregate(colViewName[col], "max");
}

int DataPanel::GetColAggregate(std::string colName, std::string function)
{
    try{
        auto stmt = m_connection->PrepareStatement("select " + function + "(" + colName + ") from series");
        auto results = stmt->GetResults();
        results->NextRow();
        return results->GetInt(0);

    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error preparing or executing minimum statement\n") + e.what());
        m_top->Close(true);
        return -1;
    }
    return -1; //this will never run
}

int DataPanel::GetColMedian(const std::string& colName)
{
    int medianStrLen = std::strlen(SqlStrings::medianStr);
    int medianStrCpyLen = medianStrLen + (colName.size() - 2)*SqlStrings::numToFormatMedianStr + 1;
    std::unique_ptr<char, decltype(std::free)*> medianStrCpy {reinterpret_cast<char*>(malloc(medianStrCpyLen)), std::free };

    if(medianStrCpyLen <= std::snprintf(medianStrCpy.get(), medianStrCpyLen, SqlStrings::medianStr,
                                        colName.c_str(), colName.c_str(), colName.c_str(), colName.c_str(), colName.c_str(), colName.c_str())){
        wxMessageBox("Failed to format median sql string");
        m_top->Close(true);
    }
    try{
        auto stmt = m_connection->PrepareStatement(medianStrCpy.get(), medianStrCpyLen);
        auto result = stmt->GetResults();
        result->NextRow();
        return result->GetInt(0);

    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error preparing or executing median statement.\n") + e.what() + "\n" + medianStrCpy.get());
        m_top->Close(true);
        return -1;
    }
    return -1; //this will never run
}

void DataPanel::RefreshGridColors()
{
    for(int i = 0; i < m_grid->GetNumberRows()-1; ++i){
        for(int k = col::FIRST_VISIBLE_COL; k < m_grid->GetNumberCols(); ++k){
            UpdateCellColor(i, k);
        }
    }
}

void DataPanel::RefreshColColors(int col)
{
    for(int i = 0; i < m_grid->GetNumberRows()-1; ++i){
        UpdateCellColor(i, col);
    }
}

void DataPanel::RefreshFilter()
{
    ApplyFilter();
}

void DataPanel::ShowSqliteBusyErrorBox()
{
    wxMessageBox("Error applying change.\nThe database is locked because its in use by another program.");
}

QuickFilter* DataPanel::GetQuickFilter()
{
    return m_quickFilter;
}
