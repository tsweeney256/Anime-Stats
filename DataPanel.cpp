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
#include "DataPanel.hpp"
#include "AppIDs.hpp"
#include "cppw/Sqlite3.hpp"
#include "GridCellDateEditor.hpp"
#include "AdvFilterFrame.hpp"

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
    EVT_BUTTON(ID_REFRESH_BTN, DataPanel::OnRefresh)
    EVT_BUTTON(ID_ADD_ROW_BTN, DataPanel::OnAddRow)
    EVT_BUTTON(ID_DELETE_ROW_BTN, DataPanel::OnDeleteRow)
    EVT_GRID_COL_SORT(DataPanel::OnGridColSort)
    EVT_GRID_CELL_CHANGING(DataPanel::OnGridCellChanging)
    EVT_COMBOBOX_DROPDOWN(wxID_ANY, DataPanel::OnComboDropDown)
    EVT_WINDOW_DESTROY(DataPanel::OnAdvFilterFrameDestruction)
END_EVENT_TABLE()

DataPanel::DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, wxWindow* top, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
		: wxPanel(parent, id, pos, size, style, name), m_top(top), m_connection(connection)
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
	auto refreshButton = new wxButton(topBar, ID_REFRESH_BTN, "Refresh");
	auto addRowButton = new wxButton(topBar, ID_ADD_ROW_BTN, "Add Row");
	auto deleteRowButton = new wxButton(topBar, ID_DELETE_ROW_BTN, "Delete Rows");

	//
	//top bar sizers
	//
	auto checkBoxSizer = new wxGridSizer(3, 5, 5);
	auto checkBoxSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, topBar, _("Filter Watched Status"));
	auto titleFilterSizer = new wxStaticBoxSizer(wxHORIZONTAL, topBar, _("Filter Title"));
	auto topControlBarSizer = new wxBoxSizer(wxHORIZONTAL);
	m_panelSizer = new wxBoxSizer(wxVERTICAL);
	auto btnSizer = new wxGridSizer(2, 3, 0, 0);

	auto checkBoxFlags = wxSizerFlags(1).Expand();
	checkBoxSizerOutline->Add(checkBoxSizer, wxSizerFlags(1).Border(wxALL).Expand());
	checkBoxSizer->Add(m_watchedCheck, checkBoxFlags);
	checkBoxSizer->Add(m_stalledCheck, checkBoxFlags);
	checkBoxSizer->Add(m_blankCheck, checkBoxFlags);
	checkBoxSizer->Add(m_watchingCheck, checkBoxFlags);
	checkBoxSizer->Add(m_droppedCheck, checkBoxFlags);
	checkBoxSizer->Add(m_allCheck, checkBoxFlags);

	titleFilterSizer->Add(m_titleFilterTextField, wxSizerFlags(1).Border(wxALL).Center());

	auto btnFlags = wxSizerFlags(1).Bottom().Expand();
	//row 1
	btnSizer->Add(applyFilterButton, btnFlags);
	btnSizer->Add(m_advFilterButton, btnFlags);
	btnSizer->Add(addRowButton, btnFlags);
	//row 2
	btnSizer->Add(resetFilterButton, btnFlags);
	btnSizer->Add(refreshButton, btnFlags);
	btnSizer->Add(deleteRowButton, btnFlags);

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

void DataPanel::SetUnsavedChanges(bool unsavedChanges) { m_unsavedChanges = unsavedChanges; }

void DataPanel::Undo()
{
    try{
        if(m_commandLevel > 0){
            m_commands[--m_commandLevel]->UnExecute();
            HandleUndoRedoColorChange();
        }
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error while undoing action.\n" + e.GetErrorMessage());
        m_top->Close(true);
    }
}

void DataPanel::Redo()
{
    try{
        if(m_commandLevel <= static_cast<int>(m_commands.size()) - 1){
            m_commands[m_commandLevel++]->Execute();
            HandleUndoRedoColorChange();
        }
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error while redoing action.\n" + e.GetErrorMessage());
        m_top->Close(true);
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
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
    m_allCheck->Disable();
    m_titleFilterTextField->SetValue("");
    NewBasicFilter();
}

void DataPanel::OnAdvFilter(wxCommandEvent& WXUNUSED(event))
{
    //non-modal
    auto frame = new AdvFilterFrame(this, "Advanced Filtering", wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    m_advFilterButton->Disable();
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
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error deleting row(s)\n" + e.GetErrorMessage());
        m_top->Destroy();
    }
    ++m_commandLevel;
    HandleCommandChecking();
    m_unsavedChanges = true;
}

void DataPanel::OnGridColSort(wxGridEvent& event)
{
    m_curOrderCol = std::to_string(event.GetCol() + 1) + " collate nocase";
    if(m_curColSort == event.GetCol()){
        m_curOrderDir = (m_curSortAsc ? "desc" : "asc");
        m_curSortAsc = !m_curSortAsc;
    }
    else{
        m_curOrderDir = "asc";
        m_curSortAsc = true;
    }
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
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making InsertCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
        catch(SqlGridCommandException& e){
            wxMessageBox("Error: " + std::string(e.what()));
            successfulEdit = false;
            event.Veto();
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
                auto control = static_cast<wxComboBox*>(editor->GetControl());
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
                    SetWatchedStatusColor(event.GetRow(), std::string(event.GetString().utf8_str()));
                }
            }
            else{
                if(event.GetCol() == col::RATING){
                    SetRatingColor(event.GetRow(), event.GetString().ToUTF8());
                }
                newVal = std::string(event.GetString().utf8_str());
                oldVal = std::string(m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str());
            }

            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_grid, this, idSeries, newVal, oldVal, col,
                    map, 1, m_changedRows));
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making UpdateCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
        catch(SqlGridCommandException& e){
            wxMessageBox("Error: " + std::string(e.what()));
            successfulEdit = false;
            event.Veto();
        }
    }
    if(successfulEdit){
        ++m_commandLevel;
        HandleCommandChecking();
        m_unsavedChanges = true;
    }
}

void DataPanel::OnComboDropDown(wxCommandEvent& event)
{
    auto control = static_cast<wxComboBox*>(event.GetEventObject());
    m_oldCellComboIndex = std::to_string(control->GetCurrentSelection());
}

void DataPanel::OnAdvFilterFrameDestruction(wxWindowDestroyEvent& event)
{
    if(event.GetId() == ID_ADV_FILTER_FRAME)
        m_advFilterButton->Enable();
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
        //don't want to use wxGridCellNumericEditor because it doesn't allow nulls
        auto intEditor = new wxGridCellTextEditor(6); //only allow 6 length as an easy way to prevent integer overflows
        wxTextValidator intValidator(wxFILTER_INCLUDE_CHAR_LIST);
        intValidator.SetCharIncludes("0123456789");
        intEditor->SetValidator(intValidator);
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
        for(int i = 0; i < numViewCols; ++i)
            m_grid->SetColLabelValue(i, wxString::FromUTF8(results->GetColumnName(i).c_str()));
    }
    int rowPos = 0;
    while(results->NextRow()){
        m_grid->AppendRows();
        m_grid->SetRowSize(rowPos, rowSize);
        m_grid->DisableRowResize(rowPos);
        for(int i = 0; i < numViewCols; ++i){
            if(i == col::RATING)
                SetRatingColor(rowPos, results->GetString(i).c_str());
            else if(i == col::WATCHED_STATUS)
                SetWatchedStatusColor(rowPos, results->GetString(i));
            m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
        }
        ++rowPos;
    }
    //user shouldn't see idSeries key
    m_grid->HideCol(0);
    AppendLastGridRow(false);
    if(m_firstDraw){
        m_firstDraw = false;
        //only do this on startup because it's wicked slow
        m_grid->AutoSize();
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
        std::string watchedStatus = "WatchedStatus.idWatchedStatus";
        std::string releaseType = "ReleaseType.idReleaseType";
        std::string season = "Season.idSeason";
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
            //releaseType
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
                statusStr << " and (series.rating between " << newAdvFilterInfo->ratingLow << " and " <<
                    newAdvFilterInfo->ratingHigh << ") ";
            if(newAdvFilterInfo->yearEnabled)
                statusStr << " and (series.year between " << newAdvFilterInfo->yearLow << " and "
                    << newAdvFilterInfo->yearHigh << ") ";
            if(newAdvFilterInfo->epsWatchedEnabled)
                statusStr << " and (series.episodesWatched between " << newAdvFilterInfo->epsWatchedLow <<
                    " and " << newAdvFilterInfo->epsWatchedHigh <<") ";
            if(newAdvFilterInfo->totalEpsEnabled)
                statusStr << " and (series.totalEpisodes between " << newAdvFilterInfo->totalEpsLow <<
                    " and " << newAdvFilterInfo->totalEpsHigh << ") ";
            if(newAdvFilterInfo->epsRewatchedEnabled)
                statusStr << " and (series.rewatchedEpisodes between " << newAdvFilterInfo->epsRewatchedLow <<
                    " and " << newAdvFilterInfo->epsRewatchedHigh << ") ";
            if(newAdvFilterInfo->lengthEnabled)
                statusStr << " and (series.episodeLength between " << newAdvFilterInfo->lengthLow << " and " <<
                    newAdvFilterInfo->lengthHigh << ") ";
            if(newAdvFilterInfo->dateStartedEnabled){
                statusStr << " and (series.dateStarted between date('";
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
                statusStr << " and (series.dateFinished between date('";
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
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.utf8_str()) + " where Title like ? " +
                (showNothing ? " and 1 <> 1 " : statusStr.str()) + (changedRows ? GetAddedRowsSqlStr(changedRows) : "") +
                " order by " + m_curOrderCol + " "+ m_curOrderDir);
        statement->Bind(1, "%" + newBasicFilterInfo->title + "%");
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
        m_titleFilterTextField->SetValue(newBasicFilterInfo->title);
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error applying filter.\n" + e.GetErrorMessage());
        m_top->Close(true);
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

void DataPanel::ApplyFullGrid()
{
    try{
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.ToUTF8()) +
                "order by " + m_curOrderCol + " " + m_curOrderDir);
        auto results = statement->GetResults();
        ResetTable(results);
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error preparing basic select statement.\n" + e.GetErrorMessage());
        m_top->Close(true);
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
    auto stmt = m_connection->PrepareStatement(sqlStmtStr);
    auto results = stmt->GetResults();
    while(results->NextRow())
        map.emplace_back(results->GetString(0).c_str(), wxMBConvUTF8());
}

void DataPanel::SetRatingColor(int row, const char* valStr)
{
    if(valStr && valStr[0] != '\0' && m_ratingColorEnabled){ //only have color if not null or blank
        int val = atoi(valStr)-1;
        if(val > m_maxRating)
            val = m_maxRating;
        int cellColour[3];

        if(val < m_midRating){
            for(int i=0; i<3; ++i){
                cellColour[i] = m_ratingColor[ratingColor::MID][i] +
                        (m_ratingColor[ratingColor::MIN][i] - m_ratingColor[ratingColor::MID][i]) *
                        ((m_midRating-val)/static_cast<double>(m_midRating));
            }
            m_grid->SetCellBackgroundColour(row, col::RATING,
                    wxColour(cellColour[ratingColor::R], cellColour[ratingColor::G], cellColour[ratingColor::B]));
        }
        else{
            for(int i=0; i<3; ++i){
                cellColour[i] = m_ratingColor[ratingColor::MAX][i] -
                        (m_ratingColor[ratingColor::MAX][i] - m_ratingColor[ratingColor::MID][i]) *
                        ((m_maxRating-val)/static_cast<double>(m_midRating-1));
            }
            m_grid->SetCellBackgroundColour(row, col::RATING,
                    wxColour(cellColour[ratingColor::R], cellColour[ratingColor::G], cellColour[ratingColor::B]));
        }
    }
    else{
        m_grid->SetCellBackgroundColour(row, col::RATING, wxColour(255, 255, 255));
    }
}

void DataPanel::SetWatchedStatusColor(int row, const std::string& valStr)
{
    if(m_watchedStatusColorEnabled){
        int idx = -1;
        //can't get the index of the combobox control because they possibly haven't been created yet and it'd be way
        //more inefficient to pre-create a combobox for every cell than to just do a few string comparisons for each one
        for(unsigned int i = 0; i < m_allowedWatchedVals.size(); ++i)
            if(!valStr.compare(m_allowedWatchedVals[i])){
                idx = i;
                break;
            }
        wxASSERT_MSG(idx > -1, "Illegal Watched Status value.");
        m_grid->SetCellBackgroundColour(row, col::WATCHED_STATUS, wxColour(m_watchedStatusColor[idx]));
    }
}

void DataPanel::HandleUndoRedoColorChange()
{
    auto rows = m_grid->GetSelectedRows();
    for(unsigned int i = 0; i < rows.Count(); ++i){
        SetRatingColor(rows.Item(i), m_grid->GetCellValue(rows.Item(i), col::RATING).ToUTF8());
        SetWatchedStatusColor(rows.Item(i), std::string(m_grid->GetCellValue(rows.Item(i), col::WATCHED_STATUS).utf8_str()));
    }
    auto cells = m_grid->GetSelectionBlockTopLeft();
    if(cells.Count() == 1){ //there should never be more than one cell selected from undoing/redoing
        if(cells.Item(0).GetCol() == col::RATING){
            SetRatingColor(cells.Item(0).GetRow(), m_grid->GetCellValue(cells.Item(0).GetRow(), col::RATING).ToUTF8());
        }
        else if(cells.Item(0).GetCol() == col::WATCHED_STATUS){
            SetWatchedStatusColor(cells.Item(0).GetRow(),
                    std::string(m_grid->GetCellValue(cells.Item(0).GetRow(), col::WATCHED_STATUS).utf8_str()));
        }
    }
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
            output += " Series.idSeries=" + std::string((*changedRows)[i].utf8_str()) + " or ";
        }
        output += " Series.idSeries=" + std::string(changedRows->back().utf8_str()) + ")";
    }
    return output;
}
