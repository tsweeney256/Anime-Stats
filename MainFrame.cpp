#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include <wx/sizer.h>
#include <wx/wxsqlite3.h>
#include "MainFrame.hpp"
#include "DataPanel.hpp"


wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)

wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
		: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	//
	//menuBar
	//
	auto menuBar = new wxMenuBar;
	SetMenuBar(menuBar);

	auto fileMenu = new wxMenu;
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_EXIT);

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(helpMenu, _("&Help"));

	//
	//noteBook
	//
	auto mainPanel = new wxPanel(this, wxID_ANY);
	auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
	auto notebook = new wxNotebook(mainPanel, wxID_ANY);
	auto dataPage = new DataPanel(notebook);
	notebook->AddPage(dataPage, _("Data"));
	mainPanelSizer->Add(notebook, wxSizerFlags(0).Expand());
	mainPanel->SetSizerAndFit(mainPanelSizer);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	Destroy();
}

void MainFrame::OnSave(wxCommandEvent &event)
{

}

void MainFrame::OnExit(wxCommandEvent &event)
{
	Close(false);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
	wxAboutDialogInfo info;
	info.SetName(_("Anime Stats"));
	info.SetVersion("0.02");
	info.SetDescription(_("Helps keep track of anime stats"));
	info.SetCopyright("(C) 2015 Thomas Sweeney");

	wxAboutBox(info);
}
