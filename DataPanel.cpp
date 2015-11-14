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
    EVT_BUTTON(ID_ADD_ROW_BTN, DataPanel::OnAddRow)
    EVT_BUTTON(ID_DELETE_ROW_BTN, DataPanel::OnDeleteRow)
    EVT_GRID_COL_SORT(DataPanel::OnGridColSort)
    EVT_GRID_CELL_CHANGING(DataPanel::OnGridCellChanging)
    EVT_COMBOBOX_DROPDOWN(wxID_ANY, DataPanel::OnComboDropDown)
END_EVENT_TABLE()

DataPanel::DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, wxWindow* top, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name)
		: wxPanel(parent, id, pos, size, style, name), m_top(top), m_connection(connection)
{
    ////
    ////Top Bar
    ////

    //
    //checkboxes
    //
	m_watchedCheck = new wxCheckBox(this, ID_WATCHED_CB, _("Watched"));
	m_watchingCheck = new wxCheckBox(this, ID_WATCHING_CB, _("Watching"));
	m_stalledCheck = new wxCheckBox(this, ID_STALLED_CB, _("Stalled"));
	m_droppedCheck = new wxCheckBox(this, ID_DROPPED_CB, _("Dropped"));
	m_blankCheck = new wxCheckBox(this, ID_BLANK_CB,  _("Blank"));
	m_allCheck = new wxCheckBox(this, ID_ALL_CB, _("Enable All"));

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
	auto applyFilterButton = new wxButton(this, ID_APPLY_FILTER_BTN, "Apply Filter");
	auto resetFilterButton = new wxButton(this, ID_RESET_FILTER_BTN, "Reset Filter");
	m_titleFilterTextField = new wxTextCtrl(this, ID_TITLE_FILTER_FIELD, wxEmptyString,
			wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);

	auto addRowButton = new wxButton(this, ID_ADD_ROW_BTN, "Add Row");
	auto deleteRowButton = new wxButton(this, ID_DELETE_ROW_BTN, "Delete Rows");

	//
	//top bar sizers
	//
	auto checkBoxSizer = new wxGridSizer(3, 5, 5);
	auto checkBoxSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter Watched Status"));
	auto titleFilterSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter Title"));
	auto topControlBarSizer = new wxBoxSizer(wxHORIZONTAL);
	auto panelSizer = new wxBoxSizer(wxVERTICAL);
	auto btnSizer = new wxGridSizer(2, 2, 0, 0);

	checkBoxSizerOutline->Add(checkBoxSizer, wxSizerFlags(0).Border(wxALL, 5));
	checkBoxSizer->Add(m_watchedCheck, wxSizerFlags(0));
	checkBoxSizer->Add(m_stalledCheck, wxSizerFlags(0));
	checkBoxSizer->Add(m_blankCheck, wxSizerFlags(0));
	checkBoxSizer->Add(m_watchingCheck, wxSizerFlags(0));
	checkBoxSizer->Add(m_droppedCheck, wxSizerFlags(0));
	checkBoxSizer->Add(m_allCheck, wxSizerFlags(0));

	titleFilterSizer->Add(m_titleFilterTextField, wxSizerFlags(0).Border(wxALL, 5).Center());

	btnSizer->Add(applyFilterButton, wxSizerFlags(0).Bottom().Expand());
	btnSizer->Add(addRowButton, wxSizerFlags(0).Bottom().Expand());
	btnSizer->Add(resetFilterButton, wxSizerFlags(0).Bottom().Expand());
	btnSizer->Add(deleteRowButton, wxSizerFlags(0).Bottom().Expand());

	topControlBarSizer->Add(checkBoxSizerOutline, wxSizerFlags(0).Bottom().Border(wxRIGHT|wxLEFT, 2));
	topControlBarSizer->Add(titleFilterSizer, wxSizerFlags(0).Bottom().Expand().Border(wxRIGHT|wxLEFT, 2));
	topControlBarSizer->Add(btnSizer, wxSizerFlags(0).Bottom().Border(wxRIGHT|wxLEFT, 2).Expand());

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
	panelSizer->Add(topControlBarSizer, wxSizerFlags(0).Border(wxALL, 2));
	panelSizer->Add(m_grid, wxSizerFlags(1).Expand().Border(wxALL, 0));
	SetSizerAndFit(panelSizer);
}

bool DataPanel::UnsavedChangesExist() { return m_unsavedChanges; }

void DataPanel::SetUnsavedChanges(bool unsavedChanges) { m_unsavedChanges = unsavedChanges; }

void DataPanel::Undo()
{
    try{
        if(m_commandLevel > 0){
            m_commands[--m_commandLevel]->UnExecute();
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
        }
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error while redoing action.\n" + e.GetErrorMessage());
        m_top->Close(true);
    }
}

void DataPanel::OnGeneralWatchedStatusCheckbox(wxCommandEvent& event)
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

void DataPanel::OnEnableAllCheckbox(wxCommandEvent& event)
{
    m_allCheck->Disable();
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
}

void DataPanel::OnTextEnter(wxCommandEvent& event)
{
    NewFilter();
}

void DataPanel::OnApplyFilter(wxCommandEvent& event)
{
    NewFilter();
}

void DataPanel::OnResetFilter(wxCommandEvent& event)
{
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_blankCheck->SetValue(true);
    m_allCheck->Disable();
    m_titleFilterTextField->SetValue("");
    NewFilter();
}

void DataPanel::OnAddRow(wxCommandEvent& event)
{
    m_grid->GoToCell(m_grid->GetNumberRows()-1, col::TITLE);
}

void DataPanel::OnDeleteRow(wxCommandEvent& event)
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
    ApplyFilterEasy();
}

void DataPanel::OnGridCellChanging(wxGridEvent& event)
{
    //if adding a new entry
    if(event.GetRow() == m_grid->GetNumberRows()-1 && event.GetCol() == col::TITLE){
        try{
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_grid, std::string(event.GetString().utf8_str()), 1));
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making InsertCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
        AppendLastGridRow(true);
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
                if(event.GetCol() == col::SEASON)
                    map = &m_allowedSeasonVals;
                if(event.GetCol() == col::WATCHED_STATUS)
                    map = &m_allowedWatchedVals;
            }
            else if(event.GetCol() == col::RATING){
                SetRatingColor(event.GetRow(), event.GetString().ToUTF8());
            }
            else{
                newVal = std::string(event.GetString().utf8_str());
                oldVal = std::string(m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str());
            }

            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_grid, idSeries, newVal, oldVal, col, map));
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making UpdateCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
    }
    ++m_commandLevel;
    HandleCommandChecking();
    m_unsavedChanges = true;
}

void DataPanel::OnComboDropDown(wxCommandEvent& event)
{
    auto control = static_cast<wxComboBox*>(event.GetEventObject());
    m_oldCellComboIndex = std::to_string(control->GetCurrentSelection());
}


void DataPanel::ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results)
{
    wxGridUpdateLocker lock;
    for(int i = m_grid->GetNumberRows() - 1; i >= 0; --i){
        m_grid->DeleteRows(i);
    }
    if(results->NextRow()){
        int rowPos = 0;
        wxASSERT_MSG(results->GetColumnCount() == numViewCols, "Basic Select Results have wrong number of columns.");
        m_grid->AppendRows();
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
        for(int i = 0; i < numViewCols; ++i){
            if(i == col::RATING)
                SetRatingColor(rowPos, results->GetString(i).c_str());
            else if(i == col::WATCHED_STATUS)
                SetWatchedStatusColor(rowPos, results->GetString(i));
            m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
        }
        ++rowPos;
        while(results->NextRow()){
            m_grid->AppendRows();
            for(int i = 0; i < numViewCols; ++i){
                if(i == col::RATING)
                    SetRatingColor(rowPos, results->GetString(i).c_str());
                else if(i == col::WATCHED_STATUS)
                    SetWatchedStatusColor(rowPos, results->GetString(i));
                m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
            }
            ++rowPos;
        }
    }
    AppendLastGridRow(false);
    //user shouldn't see idSeries key
    m_grid->HideCol(0);
    m_grid->AutoSize();
}

void DataPanel::ApplyFilter(const std::string& filterStr, bool watched, bool watching, bool stalled, bool dropped, bool blank)
{
    try{
        //setting up the where part of the sql statement to filter by watched statuses
        bool firstStatus = true;
        std::string statusStr;
        if(watched)
            AppendStatusStr(statusStr, "= 0 ", firstStatus);
        if(watching)
            AppendStatusStr(statusStr, "= 1 ", firstStatus);
        if(stalled)
            AppendStatusStr(statusStr, "= 2 ", firstStatus);
        if(dropped)
            AppendStatusStr(statusStr, "= 3 ", firstStatus);
        if(blank)
            AppendStatusStr(statusStr, " is null ", firstStatus);
        if(!firstStatus)
            statusStr += " ) ";
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.utf8_str()) +
                    " where Title like '%" + filterStr + "%'" +
                    statusStr + " order by " + m_curOrderCol + " "+ m_curOrderDir);
        auto results = statement->GetResults();
        ResetTable(results);
        m_watchedCheck->SetValue(watched);
        m_watchingCheck->SetValue(watching);
        m_stalledCheck->SetValue(stalled);
        m_droppedCheck->SetValue(dropped);
        m_blankCheck->SetValue(blank);
        if(watched && watching && stalled && dropped && blank){
            m_allCheck->SetValue(true);
            m_allCheck->Disable();
        }
        else{
            m_allCheck->SetValue(false);
            m_allCheck->Enable();
        }
        m_titleFilterTextField->SetValue(filterStr);
        Fit();
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error applying filter.\n" + e.GetErrorMessage());
        m_top->Close(true);
    }
}

void DataPanel::AppendStatusStr(std::string& statusStr, std::string toAppend, bool& firstStatus)
{
    if(!firstStatus)
        statusStr += " or ";
    else{
        statusStr += " and ( ";
        firstStatus = false;
    }
    statusStr += "WatchedStatus.idWatchedStatus" + toAppend;
}

void DataPanel::ApplyFullGrid()
{
    try{
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.ToUTF8()) +
                "order by " + m_curOrderCol + " " + m_curOrderDir);
        auto results = statement->GetResults();
        ResetTable(results);
        Fit();
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

void DataPanel::ApplyFilterEasy()
{
    ApplyFilter(std::string(m_titleFilterTextField->GetValue().utf8_str()),
            m_watchedCheck->GetValue(),
            m_watchingCheck->GetValue(),
            m_stalledCheck->GetValue(),
            m_droppedCheck->GetValue(),
            m_blankCheck->GetValue());
}

void DataPanel::NewFilter()
{
    std::string newFilterStr = std::string(m_titleFilterTextField->GetValue().utf8_str());
    m_commands.push_back(std::make_unique<FilterCommand>(this, newFilterStr, m_oldFilterStr, m_watchedCheck->GetValue(),
            m_watchingCheck->GetValue(), m_stalledCheck->GetValue(), m_droppedCheck->GetValue(), m_blankCheck->GetValue(),
            m_oldWatched, m_oldWatching, m_oldStalled, m_oldDropped, m_oldBlank));

    m_oldFilterStr = newFilterStr;
    m_oldWatched = m_watchedCheck->GetValue();
    m_oldWatching = m_watchingCheck->GetValue();
    m_oldStalled = m_stalledCheck->GetValue();
    m_oldDropped = m_droppedCheck->GetValue();
    m_oldBlank = m_blankCheck->GetValue();
    ++m_commandLevel;
    HandleCommandChecking();
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
