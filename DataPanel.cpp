#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include "DataPanel.hpp"
#include "AppIDs.hpp"

DataPanel::DataPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
		long style, const wxString& name)
		: wxPanel(parent, id, pos, size, style, name)
{
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

	auto applyFilterButton = new wxButton(this, ID_APPLY_FILTER_BTN, "Apply Filter");
	auto resetFilterButton = new wxButton(this, ID_RESET_FILTER_BTN, "Reset Filter");
	m_titleFilterTextField = new wxTextCtrl(this, ID_TITLE_FILTER_FIELD, wxEmptyString,
			wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);

	auto addRowButton = new wxButton(this, ID_ADD_ROW_BTN, "Add Row");
	auto deleteRowButton = new wxButton(this, ID_DELETE_ROW_BTN, "Delete Rows");

	//m_grid = new DataGrid(data, this, ID_DATA_GRID);

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

	panelSizer->Add(topControlBarSizer, wxSizerFlags(0).Border(wxALL, 2));
	//panelSizer->Add(m_grid, wxSizerFlags(1).Expand().Border(wxALL, 0));
	SetSizerAndFit(panelSizer);
}




