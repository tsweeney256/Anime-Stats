#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/debug.h>
#include <wx/msgdlg.h>
#include "DataPanel.hpp"
#include "AppIDs.hpp"
#include "cppw/Sqlite3.hpp"

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
        if(m_commandLevel >= 0){
            m_commands[m_commandLevel--]->UnExecute();
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
        if(m_commandLevel < static_cast<int>(m_commands.size()) - 1){
            m_commands[++m_commandLevel]->Execute();
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
    if(event.GetRow() == m_grid->GetNumberRows()-1 && event.GetCol() == col::TITLE){
        try{
            m_commands.push_back(std::make_unique<InsertCommand>(m_connection, m_grid, std::string(event.GetString().utf8_str()), 1));
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making InsertCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
        AppendLastGridRow();
    }
    else{
        try{
            auto idSeries = strtoll(m_grid->GetCellValue(event.GetRow(), col::ID_SERIES).ToUTF8(), nullptr, 10);
            auto newVal = std::string(event.GetString().utf8_str());
            auto oldVal = std::string(m_grid->GetCellValue(event.GetRow(), event.GetCol()).utf8_str());
            auto col = event.GetCol();
            m_commands.push_back(std::make_unique<UpdateCommand>(m_connection, m_grid, idSeries, newVal, oldVal, col));
        }
        catch(cppw::Sqlite3Exception& e){
            wxMessageBox("Error making UpdateCommand.\n" + e.GetErrorMessage());
            m_top->Close(true);
        }
    }
    ++m_commandLevel;
    m_unsavedChanges = true;
}

void DataPanel::ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results)
{
    wxGridUpdateLocker lock;
    if(int numRows = m_grid->GetNumberRows()) //on very first grid creation
        m_grid->DeleteRows(0, numRows);
    if(results->NextRow()){
        int rowPos = 0;
        wxASSERT_MSG(results->GetColumnCount() == numViewCols, "Basic Select Results have wrong number of columns.");
        m_grid->AppendRows();
        if(!m_colsCreated){
            m_colsCreated = true;
            m_grid->AppendCols(numViewCols);
            for(int i = 0; i < numViewCols; ++i)
                m_grid->SetColLabelValue(i, wxString::FromUTF8(results->GetColumnName(i).c_str()));
        }
        for(int i = 0; i < numViewCols; ++i)
            m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
        ++rowPos;
        while(results->NextRow()){
            m_grid->AppendRows();
            for(int i = 0; i < numViewCols; ++i){
                m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
            }
            ++rowPos;
        }
    }
    AppendLastGridRow();
    //number of rows and table keys. user shouldn't see these.
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

void DataPanel::AppendLastGridRow()
{
    m_grid->AppendRows();
    for(int i = 0; i < m_grid->GetNumberRows(); ++i){
        m_grid->SetRowLabelValue(i, wxString::Format("%i", i+1));
    }
    for(int i = col::TITLE + 1; i < numViewCols; ++i){ //want to only allow the user to edit the name field of the new entry line at first
        if(m_grid->GetNumberRows() > 1){
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
}
