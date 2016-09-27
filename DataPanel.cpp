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
#include "DataPanel.hpp"
#include "AppIDs.hpp"
#include "cppw/Sqlite3.hpp"
#include "CustomGridCellEditors.hpp"
#include "AdvFilterFrame.hpp"
#include "AdvSortFrame.hpp"
#include "TitleAliasDialog.hpp"
#include "MainFrame.hpp"
#include "Settings.hpp"
#include "SqlStrings.hpp"

BEGIN_EVENT_TABLE(DataPanel, wxPanel)
    EVT_CHECKBOX(ID_WATCHED_CB, DataPanel::OnGeneralWatchedStatusCheckbox)
    EVT_CHECKBOX(ID_WATCHING_CB, DataPanel::OnGeneralWatchedStatusCheckbox)
    EVT_CHECKBOX(ID_STALLED_CB, DataPanel::OnGeneralWatchedStatusCheckbox)
    EVT_CHECKBOX(ID_DROPPED_CB, DataPanel::OnGeneralWatchedStatusCheckbox)
    EVT_CHECKBOX(ID_BLANK_CB, DataPanel::OnGeneralWatchedStatusCheckbox)
    EVT_CHECKBOX(ID_ALL_CB, DataPanel::OnEnableAllCheckbox)
    EVT_TEXT_ENTER(ID_TITLE_FILTER_FIELD, DataPanel::OnTextEnter)
    EVT_BUTTON(ID_APPLY_FILTER_BTN, DataPanel::OnApplyFilter)
    EVT_BUTTON(ID_RESET_FILTER_BTN, DataPanel::OnResetFilter)
    EVT_BUTTON(ID_ADV_FILTER_BTN, DataPanel::OnAdvFilter)
    EVT_BUTTON(ID_ADV_SORT_BTN, DataPanel::OnAdvSort)
    EVT_BUTTON(ID_REFRESH_BTN, DataPanel::OnRefresh)
    EVT_BUTTON(ID_ADD_ROW_BTN, DataPanel::OnAddRow)
    EVT_BUTTON(ID_DELETE_ROW_BTN, DataPanel::OnDeleteRow)
    EVT_BUTTON(ID_TITLE_ALIAS_BTN, DataPanel::OnAliasTitle)
    EVT_GRID_COL_SORT(DataPanel::OnGridColSort)
    EVT_GRID_CELL_CHANGING(DataPanel::OnGridCellChanging)
    EVT_GRID_CELL_CHANGED(DataPanel::OnGridCellChanged)
    EVT_COMBOBOX_DROPDOWN(wxID_ANY, DataPanel::OnComboDropDown)
    EVT_WINDOW_DESTROY(DataPanel::OnAdvrFrameDestruction)
    EVT_GRID_LABEL_RIGHT_CLICK(DataPanel::OnLabelContextMenu)
    EVT_MENU_RANGE(ID_VIEW_COL_BEGIN, ID_VIEW_COL_END, DataPanel::OnLabelContextMenuItem)
END_EVENT_TABLE()

DataPanel::DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top, Settings* settings,
                     wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxPanel(parent, id, pos, size, style, name), m_top(top), m_settings(settings), m_connection(connection)
{
    ////
    ////Top Bar
    ////

    auto topBar = new wxScrolledWindow(this, wxID_ANY);
    topBar->SetScrollRate(10, 10);

    //
    //checkboxes
    //
    m_watchedCheck = new wxCheckBox(topBar, ID_WATCHED_CB, _("Watched"));
    m_watchingCheck = new wxCheckBox(topBar, ID_WATCHING_CB, _("Watching"));
    m_stalledCheck = new wxCheckBox(topBar, ID_STALLED_CB, _("Stalled"));
    m_droppedCheck = new wxCheckBox(topBar, ID_DROPPED_CB, _("Dropped"));
    m_blankCheck = new wxCheckBox(topBar, ID_BLANK_CB,  _("Blank"));
    m_allCheck = new wxCheckBox(topBar, ID_ALL_CB, _("Enable All"));

    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
    m_allCheck->SetValue(true);
    m_allCheck->Disable();

    //
    //buttons and textfield
    //
    m_titleFilterTextField = new wxTextCtrl(topBar, ID_TITLE_FILTER_FIELD, wxEmptyString,
                                            wxDefaultPosition, wxSize(250, -1), wxTE_PROCESS_ENTER);
    auto applyFilterButton = new wxButton(topBar, ID_APPLY_FILTER_BTN, "Apply Filter");
    auto resetFilterButton = new wxButton(topBar, ID_RESET_FILTER_BTN, "Reset Filter");
    m_advFilterButton = new wxButton(topBar, ID_ADV_FILTER_BTN, "Adv. Filter");
    m_advSortButton = new wxButton(topBar, ID_ADV_SORT_BTN, "Adv. Sort");
    auto addRowButton = new wxButton(topBar, ID_ADD_ROW_BTN, "Add Row");
    auto deleteRowButton = new wxButton(topBar, ID_DELETE_ROW_BTN, "Delete Rows");
    auto titleAliasButton = new wxButton(topBar, ID_TITLE_ALIAS_BTN, "Alias Title");
    auto refreshButton = new wxButton(topBar, ID_REFRESH_BTN, "Refresh");

    //
    //top bar sizers
    //
    auto checkBoxSizer = new wxGridSizer(3, 5, 5);
    auto checkBoxSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, topBar, _("Filter Watched Status"));
    auto titleFilterSizer = new wxStaticBoxSizer(wxHORIZONTAL, topBar, _("Filter Title"));
    auto topControlBarSizer = new wxBoxSizer(wxHORIZONTAL);
    m_panelSizer = new wxBoxSizer(wxVERTICAL);
    auto btnSizer = new wxGridSizer(2, 4, 0, 0);

    auto checkBoxFlags = wxSizerFlags(1).Expand();
    checkBoxSizerOutline->Add(checkBoxSizer, wxSizerFlags(1).Border(wxALL).Expand());
    checkBoxSizer->Add(m_watchedCheck, checkBoxFlags);
    checkBoxSizer->Add(m_stalledCheck, checkBoxFlags);
    checkBoxSizer->Add(m_blankCheck, checkBoxFlags);
    checkBoxSizer->Add(m_watchingCheck, checkBoxFlags);
    checkBoxSizer->Add(m_droppedCheck, checkBoxFlags);
    checkBoxSizer->Add(m_allCheck, checkBoxFlags);

    titleFilterSizer->Add(m_titleFilterTextField, wxSizerFlags(1).Border(wxALL).Center());

    auto btnFlags = wxSizerFlags(1).Expand();
    //row 1
    btnSizer->Add(applyFilterButton, btnFlags);
    btnSizer->Add(m_advFilterButton, btnFlags);
    btnSizer->Add(addRowButton, btnFlags);
    btnSizer->Add(titleAliasButton, btnFlags);
    //row 2
    btnSizer->Add(resetFilterButton, btnFlags);
    btnSizer->Add(m_advSortButton, btnFlags);
    btnSizer->Add(deleteRowButton, btnFlags);
    btnSizer->Add(refreshButton, btnFlags);

    auto topControlBarFlags = wxSizerFlags(0).Bottom().Expand().Border(wxALL);
    topControlBarSizer->Add(checkBoxSizerOutline, topControlBarFlags);
    topControlBarSizer->Add(titleFilterSizer, topControlBarFlags);
    topControlBarSizer->Add(btnSizer, topControlBarFlags);
    topBar->SetSizerAndFit(topControlBarSizer);

    ////
    ////grid
    ////

    m_grid = new wxGrid(this, ID_DATA_GRID);
    m_grid->CreateGrid(0,0);

    //get basic select statement from file and prepare it
    wxString basicSelectFileName = "sql/basicSelect.sql";
    bool error = false;
    if(wxFileName::FileExists(basicSelectFileName)){
        wxFile selectFile(basicSelectFileName);
        if(!selectFile.ReadAll(&m_basicSelectString))
            error = true;
    }
    else
        error = true;
    if(error){
        wxMessageBox("Error: Could not read from sql/basicSelect.sql.");
        top->Close();
    }
    BuildAllowedValsMap(m_allowedWatchedVals, "select status from WatchedStatus order by idWatchedStatus");
    BuildAllowedValsMap(m_allowedReleaseVals, "select type from ReleaseType order by idReleaseType");
    BuildAllowedValsMap(m_allowedSeasonVals, "select season from Season order by idSeason");
    ApplyFullGrid();
    //
    //panel sizer
    //
    m_panelSizer->Add(topBar, wxSizerFlags(0).Border(wxALL, 2));
    m_panelSizer->Add(m_grid, wxSizerFlags(1).Expand().Border(wxALL, 0));
    SetSizerAndFit(m_panelSizer);

    //
    //Misc initializations
    //
    m_basicFilterInfo = BasicFilterInfo::MakeShared(); //already initialized how we want it
    m_oldBasicFilterInfo = BasicFilterInfo::MakeShared();
}

bool DataPanel::UnsavedChangesExist() { return m_unsavedChanges; }

void DataPanel::SetUnsavedChanges(bool unsavedChanges)
{
    if(unsavedChanges && !m_unsavedChanges)
        m_top->SetTitle("*" + m_top->GetTitle());
    else if(m_unsavedChanges && !unsavedChanges)
        m_top->SetTitle(m_top->GetTitle().Mid(1));
    m_unsavedChanges = unsavedChanges;
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

void DataPanel::SetAddedFilterRows(std::shared_ptr<std::vector<wxString> > changedRows)
{
    m_changedRows = changedRows;
}

void DataPanel::OnGeneralWatchedStatusCheckbox(wxCommandEvent& WXUNUSED(event))
{
    if(m_watchedCheck->GetValue() && m_watchingCheck->GetValue() && m_stalledCheck->GetValue()
       && m_droppedCheck->GetValue() && m_blankCheck->GetValue()){
        m_allCheck->SetValue(true);
        m_allCheck->Disable();
    }
    else{
        m_allCheck->SetValue(false);
        m_allCheck->Enable();
    }
}

void DataPanel::OnEnableAllCheckbox(wxCommandEvent& WXUNUSED(event))
{
    m_allCheck->Disable();
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
}

void DataPanel::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
    NewBasicFilter();
}

void DataPanel::OnApplyFilter(wxCommandEvent& WXUNUSED(event))
{
    NewBasicFilter();
}

void DataPanel::OnResetFilter(wxCommandEvent& WXUNUSED(event))
{
    ResetFilterGui();
    NewBasicFilter();
}

void DataPanel::OnAdvFilter(wxCommandEvent& WXUNUSED(event))
{
    //non-modal
    auto frame = new AdvFilterFrame(this, "Advanced Filtering", wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    m_advFilterButton->Disable();
}

void DataPanel::OnAdvSort(wxCommandEvent& WXUNUSED(event))
{
    //non-modal
    auto frame = new AdvSortFrame(this, m_colList);
    frame->Show(true);
    m_advSortButton->Disable();
}

void DataPanel::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter(m_basicFilterInfo, m_advFilterInfo, m_changedRows.get());
}

void DataPanel::OnAddRow(wxCommandEvent& WXUNUSED(event))
{
    m_grid->GoToCell(m_grid->GetNumberRows()-1, col::TITLE);
    m_grid->EnableCellEditControl(true);
}

void DataPanel::OnDeleteRow(wxCommandEvent& WXUNUSED(event))
{
    auto rows = m_grid->GetSelectedRows();
    std::vector<int64_t> idSeries(rows.GetCount());
    for(unsigned int i = 0; i < rows.GetCount(); ++i){
        auto idSeriesStr = m_grid->GetCellValue(rows.Item(i), col::ID_SERIES);
        if(idSeriesStr.compare("")) //ignore the last row
            idSeries[i] = strtoll(idSeriesStr, nullptr, 10);
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
    SetUnsavedChanges(true);
}

void DataPanel::OnAliasTitle(wxCommandEvent& WXUNUSED(event))
{
    auto rows = m_grid->GetSelectedRows();

    if(rows.size() > 1)
        wxMessageBox("Error: You may only set aliases to one title at a time.");
    else if(rows.size() == 0)
        wxMessageBox("Error: No row was selected.");
    else{
        auto aliasDlg = new TitleAliasDialog(this, wxID_ANY, m_connection,
                                             wxAtol(m_grid->GetCellValue(rows[0], col::ID_SERIES)), m_grid->GetCellValue(rows[0], col::TITLE));
        if(aliasDlg->ShowModal() == wxID_OK)
            SetUnsavedChanges(true);
    }
}

void DataPanel::OnGridColSort(wxGridEvent& event)
{
    int col = event.GetCol() + 1;
    if(event.GetCol() == col::TITLE && m_sortByPronunciation)
        col = col::PRONUNCIATION + 1;
    m_curOrderCol = std::to_string(col) + " collate nocase ";
    if(m_curColSort == event.GetCol()){
        m_curOrderDir = (m_curSortAsc ? " desc " : " asc ");
        m_curSortAsc = !m_curSortAsc;
    }
    else{
        m_curOrderDir = " asc ";
        m_curSortAsc = true;
    }
    m_curOrderCombined = m_curOrderCol + m_curOrderDir;
    m_curColSort = event.GetCol();
    ApplyFilter(m_basicFilterInfo, m_advFilterInfo, m_changedRows.get());
}

void DataPanel::OnGridCellChanging(wxGridEvent& event)
{
    bool successfulEdit = true;

    //if adding a new entry
    if(event.GetRow() == m_grid->GetNumberRows()-1 && event.GetCol() == col::TITLE){
        try{
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_grid, this, std::string(event.GetString().utf8_str()), 1,
                                                                 m_changedRows));
            AppendLastGridRow(true);

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
                auto editor = m_grid->GetCellEditor(event.GetRow(), event.GetCol());
                auto control = dynamic_cast<wxComboBox*>(editor->GetControl());
                wxASSERT(control);
                int selectedIdx = control->GetCurrentSelection();
                newVal = std::to_string(selectedIdx);
                oldVal = m_oldCellComboIndex;
                editor->DecRef();
                if(event.GetCol() == col::RELEASE_TYPE)
                    map = &m_allowedReleaseVals;
                else if(event.GetCol() == col::SEASON)
                    map = &m_allowedSeasonVals;
                else if(event.GetCol() == col::WATCHED_STATUS){
                    map = &m_allowedWatchedVals;
                }
            }
            else{
                newVal = std::string(event.GetString().utf8_str());
                oldVal = std::string(m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str());
            }

            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_grid, this, idSeries, newVal, oldVal, col,
                                                                 map, 1, m_changedRows));

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
        SetUnsavedChanges(true);
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
    m_grid->Refresh(); //needed for Windows
}

void DataPanel::OnComboDropDown(wxCommandEvent& event)
{
    auto control = dynamic_cast<wxComboBox*>(event.GetEventObject());
    wxASSERT(control);
    m_oldCellComboIndex = std::to_string(control->GetCurrentSelection());
}

void DataPanel::OnAdvrFrameDestruction(wxWindowDestroyEvent& event)
{
    if(event.GetId() == ID_ADV_FILTER_FRAME)
        m_advFilterButton->Enable();
    else if(event.GetId() == ID_ADV_SORT_FRAME)
        m_advSortButton->Enable();
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
    wxASSERT_MSG(results->GetColumnCount() == numViewCols, "Basic Select Results have wrong number of columns.");
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
                m_colList.Add(colName);
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
    AppendLastGridRow(false);
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

void DataPanel::ApplyFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
                            std::shared_ptr<AdvFilterInfo> newAdvFilterInfo, std::vector<wxString>* changedRows)
//don't ever free changedRows
{
    try{
        UpdateOldFilterData();
        m_basicFilterInfo = newBasicFilterInfo;
        m_advFilterInfo = newAdvFilterInfo;

        //setting up the where part of the sql statement to filter by watched statuses
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
        if(newBasicFilterInfo->watchedBlank)
            AppendStatusStr(statusStr, watchedStatus + "= 0 ", firstStatus);
        if(!firstStatus)
            statusStr << " ) ";
        else
            showNothing = true;

        if(newAdvFilterInfo){
	    firstStatus = true;
	    if(newAdvFilterInfo->studio.size() > 0)
		AppendStatusStr(statusStr, std::string("studio LIKE '%") + newAdvFilterInfo->studio + "%')", firstStatus);
	    
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
        }
        auto sqlStr = std::string(m_basicSelectString.utf8_str()) +
            " where 1=1 " + //just a dumb hack so I don't have to worry about when to start using 'and's and 'or's
            (showNothing ? " and 1 <> 1 " : statusStr.str()) + (changedRows ? GetAddedRowsSqlStr(changedRows) : "") +
            " order by " + m_curOrderCombined;
        auto statement = m_connection->PrepareStatement(sqlStr);
        std::string bindStr = "%" + newBasicFilterInfo->title + "%";
        statement->Bind(1, bindStr);
        statement->Bind(2, bindStr);
        auto results = statement->GetResults();
        ResetTable(results);
        m_watchedCheck->SetValue(newBasicFilterInfo->watched);
        m_watchingCheck->SetValue(newBasicFilterInfo->watching);
        m_stalledCheck->SetValue(newBasicFilterInfo->stalled);
        m_droppedCheck->SetValue(newBasicFilterInfo->dropped);
        m_blankCheck->SetValue(newBasicFilterInfo->watchedBlank);
        if(newBasicFilterInfo->watched && newBasicFilterInfo->watching && newBasicFilterInfo->stalled &&
           newBasicFilterInfo->dropped && newBasicFilterInfo->watchedBlank){
            m_allCheck->SetValue(true);
            m_allCheck->Disable();
        }
        else{
            m_allCheck->SetValue(false);
            m_allCheck->Enable();
        }
        m_titleFilterTextField->SetValue(wxString::FromUTF8(newBasicFilterInfo->title.c_str()));
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error applying filter.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::SetSort(std::string sqlSortStr)
{
    m_curColSort = -1; //reset the current sorting column for the main view
    m_curSortAsc = false; //reset the current sorting direction
    m_curOrderCombined = sqlSortStr;
    ApplyFilter(m_basicFilterInfo, m_advFilterInfo, m_changedRows.get());
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

void DataPanel::ApplyFullGrid()
{
    try{
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.ToUTF8()) +
                                                        "order by " + m_curOrderCombined);
        statement->Bind(1, "%%");
        statement->Bind(2, "%%");
        auto results = statement->GetResults();
        ResetTable(results);

    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error preparing basic select statement.\n") + e.what());
        m_top->Close(true);
        return;
    }
}

void DataPanel::AppendLastGridRow(bool whiteOutPrevious)
{
    m_grid->AppendRows();
    for(int i = 0; i < m_grid->GetNumberRows(); ++i){
        m_grid->SetRowLabelValue(i, wxString::Format("%i", i+1));
    }
    for(int i = col::TITLE + 1; i < numViewCols; ++i){ //want to only allow the user to edit the name field of the new entry line at first
        if(m_grid->GetNumberRows() > 1 && whiteOutPrevious){
            m_grid->SetReadOnly(m_grid->GetNumberRows()-2, i, false);
            m_grid->SetCellBackgroundColour(m_grid->GetNumberRows()-2, i, wxColour(255, 255, 255)); //make greyed out cells white again
            m_grid->SetRowLabelValue(m_grid->GetNumberRows()-2, wxString::Format("%i", m_grid->GetNumberRows()-1));
        }

        m_grid->SetReadOnly(m_grid->GetNumberRows()-1, i);
        m_grid->SetCellBackgroundColour(m_grid->GetNumberRows()-1, i, wxColour(220, 220, 220)); //grey as a sign that the cells are read only
        m_grid->SetRowLabelValue(m_grid->GetNumberRows()-1, "*");
    }
}

void DataPanel::NewFilter(std::shared_ptr<BasicFilterInfo> newBasicFilterInfo,
                          std::shared_ptr<AdvFilterInfo> newAdvFilterInfo)
{
    m_basicFilterInfo = newBasicFilterInfo;
    m_advFilterInfo = newAdvFilterInfo;
    m_commands.push_back(std::make_unique<FilterCommand>(this, m_basicFilterInfo, m_oldBasicFilterInfo,
                                                         m_advFilterInfo, m_oldAdvFilterInfo, m_changedRows));
    UpdateOldFilterData();
    ++m_commandLevel;
    HandleCommandChecking();
}

void DataPanel::NewBasicFilter()
{
    m_basicFilterInfo = BasicFilterInfo::MakeShared();
    m_basicFilterInfo->title = std::string(m_titleFilterTextField->GetValue().utf8_str());
    m_basicFilterInfo->watched = m_watchedCheck->GetValue();
    m_basicFilterInfo->watching = m_watchingCheck->GetValue();
    m_basicFilterInfo->stalled = m_stalledCheck->GetValue();
    m_basicFilterInfo->dropped = m_droppedCheck->GetValue();
    m_basicFilterInfo->watchedBlank = m_blankCheck->GetValue();
    NewFilter(m_basicFilterInfo, nullptr);
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
        int col = cells.Item(0).GetCol();
        if(col::isColNumeric(col)){
            UpdateNumericalCellColorInfo(col);
            RefreshColColors(col);

        }else{
            UpdateCellColor(cells.Item(0).GetRow(), cells.Item(0).GetCol());
        }
    }
    m_grid->Refresh();
}

void DataPanel::UpdateOldFilterData()
{
    m_oldBasicFilterInfo = m_basicFilterInfo;
    m_oldAdvFilterInfo = m_advFilterInfo;
}

std::string DataPanel::GetAddedRowsSqlStr(std::vector<wxString>* changedRows)
{
    std::string output;

    if(changedRows && changedRows->size()){
        output = " or (";
        for(unsigned int i = 0; i < changedRows->size() - 1; ++i){
            output += " rightSide.idSeries=" + std::string((*changedRows)[i].utf8_str()) + " or ";
        }
        output += " rightSide.idSeries=" + std::string(changedRows->back().utf8_str()) + ")";
    }
    return output;
}

void DataPanel::SortByPronunciation(bool b)
{
    m_sortByPronunciation = b;
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

    ResetFilterGui();
    m_commands = std::vector<std::unique_ptr<SqlGridCommand>>();
    m_commandLevel = 0;
    m_changedRows = nullptr;
    m_connection = connection;
    SetUnsavedChanges(false);

    m_curOrderCol = " Title collate nocase ";
    m_curOrderDir = " asc ";
    m_curOrderCombined = m_curOrderCol + m_curOrderDir;
    m_curColSort = col::TITLE;
    m_curSortAsc = true;

    m_basicFilterInfo = BasicFilterInfo::MakeShared();
    m_oldBasicFilterInfo = BasicFilterInfo::MakeShared();
    m_oldBasicFilterInfo = nullptr;
    m_oldAdvFilterInfo = nullptr;

    UpdateCellColorInfo();
    ApplyFullGrid();
}

void DataPanel::ResetFilterGui()
{
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
    m_allCheck->Disable();
    m_titleFilterTextField->SetValue("");
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

void DataPanel::ShowSqliteBusyErrorBox()
{
    wxMessageBox("Error applying change.\nThe database is locked because its in use by another program.");
}
