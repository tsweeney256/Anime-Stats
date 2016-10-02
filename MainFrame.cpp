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
#include <unordered_map>
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
#include <wx/richmsgdlg.h>
#include <wx/xml/xml.h>
#include "MainFrame.hpp"
#include "DataPanel.hpp"
#include "ColorOptionsDlg.hpp"
#include "cppw/Sqlite3.hpp"
#ifndef NDEBUG
#include <iostream>
#endif

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
    EVT_MENU(wxID_REDO, MainFrame::OnRedo)
    EVT_MENU(DEFAULT_DB_ASK, MainFrame::OnDefaultDbAsk)
    EVT_MENU(DEFAULT_DB, MainFrame::OnDefaultDb)
    EVT_MENU(COLOR_OPTIONS, MainFrame::OnColorOptions)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(MAL_IMPORT, MainFrame::OnImportMAL)
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
    }catch(SettingsSaveException& e){
        auto status = wxMessageBox(wxString(e.what()) + "\nContinue Anyway?", "Error", wxYES_NO);
        if(status == wxNO)
            Close(true);
        return;

    }catch(SettingsLoadException& e){
        wxMessageBox(wxString(e.what()) + "\nThe program will now close.");
        Close(true);
        return;
    }
    wxSetWorkingDirectory(appDir);
    if(!wxFileName::FileExists(m_settings->defaultDb)){
        if(m_settings->defaultDb.size()){
            wxMessageBox("The default database could not be found.");
        }
        m_dbFile = ":memory:";
        m_dbInMemory = true;
    }
    else
        m_dbFile = m_settings->defaultDb;
    //
    //menuBar
    //
    auto menuBar = new wxMenuBar;
    SetMenuBar(menuBar);

    m_fileMenu = new wxMenu;
    m_fileMenu->Append(wxID_NEW);
    m_fileMenu->Append(wxID_OPEN);
    m_fileMenu->Append(wxID_SAVE);
    auto importMenu = new wxMenu;
    m_fileMenu->AppendSubMenu(importMenu, "Import");
    importMenu->Append(MAL_IMPORT, "MAL");  
            
    m_fileMenu->Append(DEFAULT_DB, _("Default Database"),
                       _("Select or unselect this file as your default database."), wxITEM_CHECK);
    m_fileMenu->Check(DEFAULT_DB, !m_dbInMemory);
    m_fileMenu->Enable(DEFAULT_DB, !m_dbInMemory);
    m_fileMenu->Append(wxID_EXIT);

    auto editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO);
    editMenu->Append(wxID_REDO);
    m_preferencesMenu = new wxMenu;
    m_preferencesMenu->Append(DEFAULT_DB_ASK, _("Always ask to change default database"),
                              _("Toggle whether or not you get asked to change your default database when you open a new one."), wxITEM_CHECK);
    m_preferencesMenu->Check(DEFAULT_DB_ASK, m_settings->defaultDbAsk);
    m_preferencesMenu->Append(COLOR_OPTIONS, _("Grid Colors"), _("Change what colors the grid uses."));
    editMenu->AppendSubMenu(m_preferencesMenu, _("Preferences"));

    auto helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    menuBar->Append(m_fileMenu, _("&File"));
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

    }catch(SettingsSaveException& e){
        wxMessageBox("Unable to save application settings.");
        return;
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
    bool savedChanges = false;
    try{
        m_connection->Commit();
        m_connection->Begin();
        savedChanges = true;
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error saving.\n") + e.what());
        return;
    }
    if(m_dbInMemory){
        savedChanges = WriteMemoryDbToFile();
        if(savedChanges){
            DoDefaultDbPopup();
            m_fileMenu->Enable(DEFAULT_DB, true);
        }
    }
    if(savedChanges){
        m_dataPanel->SetUnsavedChanges(false);
    }
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

void MainFrame::OnDefaultDbAsk(wxCommandEvent& event)
{
    m_settings->defaultDbAsk = event.IsChecked();
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
        m_dbInMemory = true;
        m_dbFile = ":memory:";
        m_fileMenu->Check(DEFAULT_DB, false);
        m_fileMenu->Enable(DEFAULT_DB, false);
        m_connection = GetDbConnection(m_dbFile);
        m_dataPanel->ResetPanel(m_connection.get());
    }
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    if(!(m_dataPanel->UnsavedChangesExist() && SaveChangesPopup() == wxID_CANCEL)){
        m_dbInMemory = false; //just in case there was a db in memory with no changes made to it
        wxString dir = wxStandardPaths::Get().GetDocumentsDir();
        wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "DB files (*.db)|*.db", wxFD_OPEN);
        int status;
        std::unique_ptr<cppw::Sqlite3Connection> newConnection;
        do{
            status = dlg.ShowModal();
            if(status == wxID_OK){
                m_dbFile = dlg.GetPath();
                newConnection = GetDbConnection(m_dbFile, false);
            }
            else break;
        }while(!newConnection);
        if(newConnection && status == wxID_OK){
            m_connection = std::move(newConnection);
            if(m_dbFile.compare(m_settings->defaultDb)){
                DoDefaultDbPopup();
            }else{
                m_fileMenu->Enable(DEFAULT_DB, true);
                m_fileMenu->Check(DEFAULT_DB, true);
            }
            m_dataPanel->ResetPanel(m_connection.get());
        }
    }
}

void MainFrame::OnImportMAL(wxCommandEvent &event)
{
    wxMessageDialog deleteCommandsDlg(this, _("This will clear your undo history.\n"
                                               "Are you sure you wish to contunue?"),
                                       wxMessageBoxCaptionStr, wxYES_NO|wxYES_DEFAULT|wxCENTER);
    if(deleteCommandsDlg.ShowModal() == wxID_NO){
        return;
    }

    auto dir = wxStandardPaths::Get().GetDocumentsDir();
    wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "XML files (*.xml)|*.xml", wxFD_OPEN);
    auto status = dlg.ShowModal();
    if(status == wxID_OK){
        auto path = dlg.GetPath();
        wxXmlDocument doc;
        if(!doc.Load(path)){
            wxMessageBox("Invalid XML file");
            return;
        }else{
            auto root = doc.GetRoot();
            if(root->GetName() != "myanimelist"){
                wxMessageBox("Unrecognized XML file type.\nExpected MAL Export XML file.");
                return;
            }
            auto child = root->GetChildren();
            if(!child){
                wxMessageBox("There was an error parsing the MAL Export file.\n"
                             "Unable to find child of root node.");
                return;
            }
            //Clear the undo history only after early detection of errors
            m_dataPanel->ClearCommandHistory();
            auto seriesStmt = m_connection->PrepareStatement(
                "INSERT INTO Series (rating, idReleaseType, idWatchedStatus, episodesWatched, totalEpisodes, "
                "rewatchedEpisodes, dateStarted, dateFinished) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            auto titleStmt = m_connection->PrepareStatement("INSERT INTO Title (name, idSeries, idLabel) VALUES (?, ?, 1)");
            
            std::unordered_map<std::string, int> allowedValsMap
            {
                {"TV",        1},
                {"OVA",       2},
                {"Movie",     4},
                {"Special",   5},
                {"ONA",       3},
                {"Watching",  2},
                {"Completed", 1},
                {"On-Hold",   3},
                {"Dropped",   4},
                {"Plan to Watch", 5}
            };
            
            do{
                if(child->GetName() == "anime"){
                    auto dataNode = child->GetChildren();
                    auto totalEpisodes = 0;
                    auto fullRewatchCount = 0;
                    auto currentRewatchCount = 0;

                    try{
                        seriesStmt->Reset();
                        seriesStmt->ClearBindings();
                        titleStmt->Reset();
                        titleStmt->ClearBindings();
                        bool recordEntry = true;
                        do{
                            const auto tag = std::string(dataNode->GetName().utf8_str());
                            const auto content = std::string(dataNode->GetNodeContent().utf8_str());
                            //could probably just always assume a certain order, but I'm being paranoid
                            if(tag == "series_title"){
                                auto dupeTitleCheckStmt = m_connection->PrepareStatement("select idName from Title where idLabel=? and name=?");
                                dupeTitleCheckStmt->Bind(1, 1);
                                dupeTitleCheckStmt->Bind(2, content);
                                if(dupeTitleCheckStmt->GetResults()->NextRow()){
                                    recordEntry = false;
                                    break;
                                }
                                titleStmt->Bind(1, content.c_str());
                            }else if(tag == "series_type"){
                                auto result = allowedValsMap.find(content);
                                if(result != allowedValsMap.end()){
                                    seriesStmt->Bind(2, result->second);
                                }
                            }else if(tag == "series_episodes"){
                                try{
                                    totalEpisodes = std::stoi(content);
                                    seriesStmt->Bind(5, totalEpisodes);
                                }catch(...){/*if invalid, it just defaults to null anyway*/}
                            }else if(tag == "my_watched_episodes"){
                                if(content != "0")
                                    seriesStmt->Bind(4, content);
                            }else if(tag == "my_start_date"){
                                if(content != "0000-00-00"){
                                    seriesStmt->Bind(7, content);
                                }
                            }else if(tag == "my_finish_date"){
                                if(content != "0000-00-00"){
                                    seriesStmt->Bind(8, content);
                                }
                            }else if(tag == "my_score"){
                                if(content != "0")
                                    seriesStmt->Bind(1, content);
                            }else if(tag == "my_status"){
                                auto result = allowedValsMap.find(content);
                                if(result != allowedValsMap.end()){
                                    seriesStmt->Bind(3, result->second);
                                }
                            }else if(tag == "my_times_watched"){
                                try{
                                    fullRewatchCount = std::stoi(content);
                                }catch(...) {/*if invalid, it just defaults to null anyway*/}
                            }else if(tag == "my_rewatching"){
                                try{
                                    currentRewatchCount = std::stoi(content);
                                }catch(...) {/*if invalid, it just defaults to null anyway*/}
                            }
                        }while((dataNode = dataNode->GetNext()));
                        if(recordEntry){
                            auto rewatchedCount = totalEpisodes * fullRewatchCount + currentRewatchCount;
                            if(rewatchedCount != 0)
                                seriesStmt->Bind(6, rewatchedCount);
                            seriesStmt->GetResults()->NextRow();
                            titleStmt->Bind(2, m_connection->GetLastInsertRowID());
                            titleStmt->GetResults()->NextRow();
                        }
                    }catch(cppw::Sqlite3Exception& e){
                        wxMessageBox(e.what());
                        return;
                    }
                }
            }while((child = child->GetNext()));
            m_dataPanel->SetUnsavedChanges(true);
            m_dataPanel->UpdateCellColorInfo();
            m_dataPanel->RefreshFilter();
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
    if(m_settings->defaultDbAsk){
        wxRichMessageDialog msgBox(this, "Do you want to make this file your default database?", "Make Default?", wxYES_NO);
        msgBox.ShowCheckBox("Never ask me again", false);
        auto status = msgBox.ShowModal();
        if(status == wxID_YES){ //picky about wxYES and wxID_YES
            m_settings->defaultDb = m_dbFile;
        }
        m_fileMenu->Check(DEFAULT_DB, status == wxID_YES);
        m_settings->defaultDbAsk = !msgBox.IsCheckBoxChecked();
        m_preferencesMenu->Check(DEFAULT_DB_ASK, !msgBox.IsCheckBoxChecked());
    }
}

int MainFrame::SaveChangesPopup()
{
    wxMessageDialog dlg(this, _("Save changes to database before closing?"),
                        wxMessageBoxCaptionStr, wxCANCEL|wxYES_NO|wxCANCEL_DEFAULT|wxCENTER);
    auto status  = dlg.ShowModal();
    try{
        if(status == wxID_YES){
            m_connection->Commit();
            m_connection->Begin();
            if(m_dbInMemory){
                if(!WriteMemoryDbToFile())
                    status = wxID_CANCEL;
            }
        }
    }catch(cppw::Sqlite3Exception& e){
        wxMessageBox(std::string("Error:\n") + e.what());
        if(status == wxID_YES) //if there is an error saving, let the user figure out what they want to do
            return wxID_CANCEL;
        //else just shrug and let the program do its thing
        return status;
    }
    return status;
}

std::unique_ptr<cppw::Sqlite3Connection> MainFrame::GetDbConnection(const wxString& file, bool eraseIfAlreadyExists)
{
    bool fileExists;
    if(!m_dbInMemory){
        fileExists = wxFileName::FileExists(file);
        if(fileExists && eraseIfAlreadyExists){
            wxRemoveFile(file);
            fileExists = false;
        }
    }
    else{
        fileExists = false;
    }
    std::unique_ptr<cppw::Sqlite3Connection> connection;
    try{
        connection = std::make_unique<cppw::Sqlite3Connection>(std::string(file.utf8_str()));
        SetDbFlags(connection.get());

    }catch(cppw::Sqlite3Exception& e) {
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

                }catch(cppw::Sqlite3Exception& e){
                    error = true;
                    errorMsg <<_("Error: Could not execute create command.\n sqlite3 error: ") << e.GetErrorCode() <<
                        _(" ") << e.what();
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
            if(!m_dbInMemory){
                wxRemoveFile(file);
            }
            wxMessageBox(errorMsg);
            Destroy();
        }
    }
    return connection;
}

void MainFrame::SetDbFlags(cppw::Sqlite3Connection* connection)
{
#ifndef NDEBUG
    connection->SetLogging(&std::cout);
#endif
    connection->EnableForeignKey(true);
    connection->Begin();
}

bool MainFrame::WriteMemoryDbToFile()
{
    wxString dir = wxStandardPaths::Get().GetDocumentsDir();
    wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "DB files (*.db)|*.db", wxFD_SAVE);
    int status;
    bool error = false;
    bool dbIsNowFile = false;
    do{
        status = dlg.ShowModal();
        if(status == wxID_OK){
            try{
                m_connection = std::make_unique<cppw::Sqlite3Connection>(
                    std::move(m_connection->BackupToFile(std::string(dlg.GetPath().utf8_str()))));
                error = false;
                dbIsNowFile = true;
                SetDbFlags(m_connection.get());
                m_dbFile = dlg.GetPath();
                m_dataPanel->SetSqlite3Connection(m_connection.get());
                m_dbInMemory = false;

            }catch(cppw::Sqlite3Exception& e){
                wxMessageBox("Error creating database.\nYour hard drive may be full or you may not have "
                             "the proper permissions to write in this folder.");
                error = true;
            }
        }
    }while(status == wxID_OK && error);
    return dbIsNowFile;
}

void MainFrame::OnColorOptions(wxCommandEvent& WXUNUSED(event))
{
    ColorOptionsDlg dlg(m_settings.get(), m_dataPanel, this);
    dlg.ShowModal();
}
