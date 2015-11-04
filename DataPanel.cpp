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
    ApplyFilter();
}

void DataPanel::OnApplyFilter(wxCommandEvent& event)
{
    ApplyFilter();
}

void DataPanel::OnResetFilter(wxCommandEvent& event)
{
    ApplyFullGrid();
    m_titleFilterTextField->SetValue("");
}

void DataPanel::OnAddRow(wxCommandEvent& event)
{
}

void DataPanel::OnDeleteRow(wxCommandEvent& event)
{
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
    ApplyFilter();
}

void DataPanel::ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results)
{
    wxGridUpdateLocker lock;
    if(int numRows = m_grid->GetNumberRows()) //on very first grid creation
        m_grid->DeleteRows(0, numRows);
    if(results->NextRow()){
        int rowPos = 0;
        wxASSERT_MSG(results->GetColumnCount() == m_numCols, "Basic Select Results have wrong number of columns.");
        m_grid->AppendRows();
        if(!m_colsCreated){
            m_colsCreated = true;
            m_grid->AppendCols(m_numCols);
            for(int i = 0; i < m_numCols; ++i)
                m_grid->SetColLabelValue(i, wxString::FromUTF8(results->GetColumnName(i).c_str()));
        }
        for(int i = 0; i < m_numCols; ++i)
            m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
        ++rowPos;
        while(results->NextRow()){
            m_grid->AppendRows();
            for(int i = 0; i < m_numCols; ++i){
                m_grid->SetCellValue(rowPos, i, wxString::FromUTF8(results->GetString(i).c_str()));
            }
            ++rowPos;
        }
    }
    m_grid->AppendRows(); //blank row at the bottom
    //number of rows and table keys. user shouldn't see these.
    m_grid->HideCol(0); m_grid->HideCol(1);
    m_grid->AutoSize();
}

void DataPanel::ApplyFilter()
{
    try{
        //setting up the where part of the sql statement to filter by watched statuses
        bool firstStatus = true;
        std::string statusStr;
        if(m_watchedCheck->GetValue())
            AppendStatusStr(statusStr, "= 0 ", firstStatus);
        if(m_watchingCheck->GetValue())
            AppendStatusStr(statusStr, "= 1 ", firstStatus);
        if(m_stalledCheck->GetValue())
            AppendStatusStr(statusStr, "= 2 ", firstStatus);
        if(m_droppedCheck->GetValue())
            AppendStatusStr(statusStr, "= 3 ", firstStatus);
        if(m_blankCheck->GetValue())
            AppendStatusStr(statusStr, " is null ", firstStatus);
        if(!firstStatus)
            statusStr += " ) ";
        auto statement = m_connection->PrepareStatement(std::string(m_basicSelectString.utf8_str()) +
                    " where Title like '%" + std::string(m_titleFilterTextField->GetValue().utf8_str()) + "%'" +
                    statusStr + " order by " + m_curOrderCol + " "+ m_curOrderDir);
        statement->Bind(1,1); //placeholder. selecting english titles only.
        auto results = statement->GetResults();
        ResetTable(results);
        Fit();
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error applying filter.\n" + e.GetErrorMessage());
        m_top->Close();
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
        statement->Bind(1,1); //placeholder. selects only english titles.
        auto results = statement->GetResults();
        ResetTable(results);
        Fit();
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error preparing basic select statement.\n" + e.GetErrorMessage());
        m_top->Close();
    }
}
