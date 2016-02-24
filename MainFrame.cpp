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
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include "MainFrame.hpp"
#include "DataPanel.hpp"
#include "cppw/Sqlite3.hpp"
#include "OpenDbDlg.hpp"
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
	EVT_MENU(SORT_BY_PRONUNCIATION, MainFrame::OnPreferencesSortByPronunciation)
	EVT_MENU(DEFAULT_DB, MainFrame::OnDefaultDb)
	EVT_MENU(wxID_NEW, MainFrame::OnNew)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
		: wxFrame(NULL, wxID_ANY, title, pos, size)
{
    //
    //settings
    //
    auto appDir = wxGetCwd();
    SwitchToDataDir();
    try{
        if(wxFileName::FileExists(settingsFileName))
            m_settings = std::make_unique<Settings>(settingsFileName);
        else{
            m_settings = std::make_unique<Settings>();
            m_settings->Save(settingsFileName);
        }
    }
    catch(SettingsSaveException& e){
        auto status = wxMessageBox(wxString(e.what()) + "\nContinue Anyway?", "Error", wxYES_NO);
        if(status == wxNO)
            Destroy();
    }
    catch(SettingsLoadException& e){
        wxMessageBox(wxString(e.what()) + "\nThe program will now close.");
    }
    wxSetWorkingDirectory(appDir);
    if(!wxFileName::FileExists(m_settings->defaultDb)){
        wxString msg;
        if(!m_settings->defaultDb.size())
            msg = "A default database has not been set.\nPlease open one or create a new one.";
        else
            msg = "The default database was not found.\nPlease open one or create a new one.";
        OpenDbDlg dlg(m_dbFile, msg, this, wxID_ANY);
        if(dlg.ShowModal() == wxID_CANCEL){
            Destroy();
            return; //I guess Destroy() refuses to run until the constructor has finished
        }
        else{
            DoDefaultDbPopup();
        }
    }
    else
        m_dbFile = m_settings->defaultDb;
    wxSetWorkingDirectory(appDir);
	//
	//menuBar
	//
	auto menuBar = new wxMenuBar;
	SetMenuBar(menuBar);

	auto fileMenu = new wxMenu;
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(DEFAULT_DB, _("Default Database"),
	        _("Select or unselect this file as your default database."), wxITEM_CHECK);
	fileMenu->Check(DEFAULT_DB, m_settings->defaultDb.size());
	fileMenu->Append(wxID_EXIT);

	auto editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO);
	editMenu->Append(wxID_REDO);
	auto preferencesMenu = new wxMenu;
	preferencesMenu->Append(SORT_BY_PRONUNCIATION, _("Sort title by pronunciation"),
	        _("Sorts the title column by the user given pronunciation instead of by its Unicode values."
	        "Useful for things like chinese characters."), wxITEM_CHECK);
	preferencesMenu->Check(SORT_BY_PRONUNCIATION, m_settings->sortingByPronunciation);
	editMenu->AppendSubMenu(preferencesMenu, _("Preferences"));

	auto helpMenu = new wxMenu;
	helpMenu->Append(wxID_ABOUT);

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));
	menuBar->Append(helpMenu, _("&Help"));

    //
    //database
    //
    m_connection = GetDbConnection(m_dbFile);
    if(!m_connection)
        return; //GetDbConnection calls Destroy, but since this is the constructor, it wont run until the construction is finished

    //
    //noteBook
    //
    auto mainPanel = new wxPanel(this, wxID_ANY);
    auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    auto notebook = new wxNotebook(mainPanel, wxID_ANY);
    m_dataPanel = new DataPanel(m_connection.get(), notebook, this, m_settings.get());
    m_dataPanel->SortByPronunciation(m_settings->sortingByPronunciation);
    notebook->AddPage(m_dataPanel, _("Data"));
    mainPanelSizer->Add(notebook, wxSizerFlags(1).Expand());
    mainPanel->SetSizerAndFit(mainPanelSizer);
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    SwitchToDataDir();
    m_dataPanel->WriteSizesToSettings();
    try{
        m_settings->Save(settingsFileName); //save the settings file no matter what
    }
    catch(SettingsSaveException& e){
        wxMessageBox("Unable to save application settings.");
    }
    if(m_dataPanel->UnsavedChangesExist() && event.CanVeto()){
        auto status = SaveChangesPopup();
        if(status != wxID_CANCEL)
            Destroy();
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

void MainFrame::OnPreferencesSortByPronunciation(wxCommandEvent& event)
{
    m_dataPanel->SortByPronunciation(event.IsChecked());
    m_settings->sortingByPronunciation = event.IsChecked();
}

void MainFrame::OnDefaultDb(wxCommandEvent& event)
{
    if(event.IsChecked())
        m_settings->defaultDb = std::string(m_dbFile.utf8_str());
    else
        m_settings->defaultDb = "";
}

void MainFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    if(!(m_dataPanel->UnsavedChangesExist() && SaveChangesPopup() == wxID_CANCEL)){
        wxString dir = wxStandardPaths::Get().GetDocumentsDir();
        wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "DB files (*.db)|*.db", wxFD_SAVE);
        int status;
        std::unique_ptr<cppw::Sqlite3Connection> newConnection;
        do{
            status = dlg.ShowModal();
            m_dbFile = dlg.GetPath();
            newConnection = GetDbConnection(m_dbFile, true);
        }while(status == wxID_OK && !newConnection);
        if(newConnection && status == wxID_OK){
            m_connection = std::move(newConnection);
            DoDefaultDbPopup();
            m_dataPanel->ResetPanel(m_connection.get());
        }
    }
}

void MainFrame::SwitchToDataDir()
{
    auto dataDir = wxStandardPaths::Get().GetUserDataDir();
    auto status = wxYES;
    if(!wxDirExists(dataDir)){
        if(!wxDir::Make(dataDir)){
            status = wxMessageBox("Unable to create application directory:\n" + dataDir + "\nDo you wish to continue anyway?");
            if(status == wxNO)
                Destroy();
        }
    }
    //skip error message if it's redundant
    if(status == wxYES && !wxSetWorkingDirectory(dataDir)){
        if(wxMessageBox("Unable to set working directory.\n" + dataDir + "Do you wish to continue anyway?.") == wxNO)
            Destroy();
    }
}

void MainFrame::DoDefaultDbPopup()
{
    auto status = wxMessageBox("Do you want to make this file your default database?", "Make Default?", wxYES_NO, this);
    if(status == wxYES)
        m_settings->defaultDb = m_dbFile;
}

int MainFrame::SaveChangesPopup()
{
    auto dlg = new wxMessageDialog(this, _("Save changes to database before closing?"),
            wxMessageBoxCaptionStr, wxCANCEL|wxYES_NO|wxCANCEL_DEFAULT|wxCENTER);
    auto status  = dlg->ShowModal();
    try{
        if(status == wxID_YES){
            m_connection->Commit();
        } else if(status == wxID_NO){
            m_connection->Rollback();
        }
    } catch(cppw::Sqlite3Exception& e){
        wxMessageBox("Error:\n" + e.GetErrorMessage());
        if(status == wxID_YES) //if there is an error saving, let the user figure out what they want to do
            return wxID_CANCEL;
        //else just shrug and let the program do its thing
        return status;
    }
    return status;
}

std::unique_ptr<cppw::Sqlite3Connection> MainFrame::GetDbConnection(const wxString& file, bool eraseIfAlreadyExists)
{
    auto fileExists = wxFileName::FileExists(file);
    if(fileExists && eraseIfAlreadyExists){
        wxRemoveFile(file);
        fileExists = false;
    }
    std::unique_ptr<cppw::Sqlite3Connection> connection;
    try{
        connection = std::make_unique<cppw::Sqlite3Connection>(std::string(file.utf8_str()));
    #ifdef NDEBUG
        connection->SetLogging(&std::cout);
    #endif
        connection->EnableForeignKey(true);
        connection->Begin();
    } catch(cppw::Sqlite3Exception& e) {
        wxMessageBox("Error creating database.\nYour hard drive may be full or you may not have "
                "the proper permissions to write in this folder.");
        return nullptr;
    }
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
                    connection->ExecuteQuery(sql);
                    connection->Commit();
                    connection->Begin();
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
            connection->Rollback();
            connection = nullptr;
            wxRemoveFile(file);
            wxMessageBox(errorMsg);
            Destroy();
        }
    }
    return connection;
}
