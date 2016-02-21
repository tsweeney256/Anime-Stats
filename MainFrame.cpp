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

#include <string>
#include <memory>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include "MainFrame.hpp"
#include "DataPanel.hpp"
#include "cppw/Sqlite3.hpp"
#ifdef NDEBUG
#include <iostream>
#endif

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
	EVT_MENU(wxID_EXIT, MainFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
	EVT_MENU(wxID_REDO, MainFrame::OnRedo)

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

	auto editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO);
	editMenu->Append(wxID_REDO);

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));
	menuBar->Append(helpMenu, _("&Help"));

    //
    //database
    //
    auto fileExists = wxFileName::FileExists("AnimeData.db");
    m_connection = std::make_unique<cppw::Sqlite3Connection>("AnimeData.db");
#ifdef NDEBUG
    m_connection->SetLogging(&std::cout);
#endif
    m_connection->EnableForeignKey(true);
    m_connection->Begin();
    if(!fileExists){
        auto error = false;
        wxString errorMsg;
        wxString createFileName = "sql/create.sql";
        wxString createStr;
        if(wxFileName::FileExists(createFileName)){
            wxFile createFile(createFileName);
            if(createFile.ReadAll(&createStr)){
                try{
                    auto sql = std::string(createStr.utf8_str());
                    m_connection->ExecuteQuery(sql);
                    m_connection->Commit();
                    m_connection->Begin();
                }
                catch(cppw::Sqlite3Exception& e){
                    error = true;
                    errorMsg <<_("Error: Could not execute create command.\n sqlite3 error: ") << e.GetExtendedErrorCode() <<
                           _(" ") << e.GetErrorMessage();
                }
            }
            else{
                error = true;
                errorMsg = "Error: Could not read from sql/create.sql.";
            }
        }
        else{
            error = true;
            errorMsg = "Error: " + createFileName + " could not be found.";
        }
        if(error){
            m_connection->Rollback();
            wxRemoveFile("AnimeStats.db");
            wxMessageBox(errorMsg);
            Close();
        }
    }

    //
    //noteBook
    //
    auto mainPanel = new wxPanel(this, wxID_ANY);
    auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    auto notebook = new wxNotebook(mainPanel, wxID_ANY);
    m_dataPanel = new DataPanel(m_connection.get(), notebook, this);
    notebook->AddPage(m_dataPanel, _("Data"));
    mainPanelSizer->Add(notebook, wxSizerFlags(1).Expand());
    mainPanel->SetSizerAndFit(mainPanelSizer);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if(m_dataPanel->UnsavedChangesExist() && event.CanVeto()){
        auto test = new wxMessageDialog(this, _("Save changes to database before closing?"),
                wxMessageBoxCaptionStr, wxCANCEL|wxYES_NO|wxCANCEL_DEFAULT|wxCENTER);
        auto button = test->ShowModal();
        if(button == wxID_YES){
            try{
                m_connection->Commit();
            }
            catch(cppw::Sqlite3Exception& e){
                wxMessageBox("Error saving.\n" + e.GetErrorMessage());
            }
            Destroy();
        }
        else if(button == wxID_NO){
            m_connection->Rollback();
            Destroy();
        }
        else //cancel
            event.Veto();
    }
    else{
        m_connection->Rollback();
        Destroy();
    }
}

void MainFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    try{
        m_connection->Commit();
        m_connection->Begin();
    }
    catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error saving.\n" + e.GetErrorMessage());
    }
    m_dataPanel->SetUnsavedChanges(false);
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(false);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName(_("Anime Stats"));
	info.SetVersion("0.02");
	info.SetDescription(_("Helps keep track of anime stats"));
	info.SetCopyright("(C) 2015 Thomas Sweeney");

	wxAboutBox(info);
}

void MainFrame::OnUndo(wxCommandEvent& WXUNUSED(event)) { m_dataPanel->Undo(); }

void MainFrame::OnRedo(wxCommandEvent& WXUNUSED(event)) { m_dataPanel->Redo(); }
