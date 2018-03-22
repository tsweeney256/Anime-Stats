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

#include <algorithm>
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
#include <wx/wupdlock.h>
#include <wx/xml/xml.h>
#include "MainFrame.hpp"
#include "DataPanel.hpp"
#include "ColorOptionsDlg.hpp"
#include "Helpers.hpp"
#include "cppw/Sqlite3.hpp"
#include "QuickFilter.hpp"
#include "TopBar.hpp"
#ifndef NDEBUG
#include <iostream>
#endif

static const int current_db_version = 4;
extern "C"{
    int sqlite3_extension_init(
        sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);
    int sqlite3_countvalue_init(
        sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_UNDO, MainFrame::OnUndo)
    EVT_MENU(wxID_REDO, MainFrame::OnRedo)
    EVT_MENU(ADD_ROW, MainFrame::OnAddRow)
    EVT_MENU(DELETE_ROWS, MainFrame::OnDeleteRows)
    EVT_MENU(ALIAS_TITLE, MainFrame::OnAliasTitle)
    EVT_MENU(EDIT_TAGS, MainFrame::OnEditTags)
    EVT_MENU(APPLY_FILTER, MainFrame::OnApplyFilter)
    EVT_MENU(DEFAULT_FILTER, MainFrame::OnDefaultFilter)
    EVT_MENU(ADV_FILTER, MainFrame::OnAdvFilter)
    EVT_MENU(ADV_SORT, MainFrame::OnAdvSort)
    EVT_MENU(MAKE_DEFAULT_FILTER, MainFrame::OnMakeDefaultFilter)
    EVT_MENU(DEFAULT_DB_ASK, MainFrame::OnDefaultDbAsk)
    EVT_MENU(DEFAULT_DB, MainFrame::OnDefaultDb)
    EVT_MENU(COLOR_OPTIONS, MainFrame::OnColorOptions)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(MAL_IMPORT, MainFrame::OnImportMAL)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::OnTabChange)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    sqlite3_auto_extension((void(*)(void))sqlite3_extension_init);
    sqlite3_auto_extension((void(*)(void))sqlite3_countvalue_init);
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
        if (!CreateMemoryDb()) {
            return;
        }
    } else {
        if (!OpenDb(m_settings->defaultDb)) {
            return;
        }
    }
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
    editMenu->Append(ADD_ROW, _("Add Row\tCTRL+Enter"));
    editMenu->Append(DELETE_ROWS, _("Delete Rows\tCTRL+SHIFT+Del"));
    editMenu->Append(ALIAS_TITLE, _("Alias Title\tCTRL+t"));
    editMenu->Append(EDIT_TAGS, _("Edit Tags\tCTRL+SHIFT+t"));
    m_preferencesMenu = new wxMenu;
    m_preferencesMenu->Append(DEFAULT_DB_ASK, _("Always ask to change default database"),
                              _("Toggle whether or not you get asked to change your default database when you open a new one."), wxITEM_CHECK);
    m_preferencesMenu->Check(DEFAULT_DB_ASK, m_settings->defaultDbAsk);
    m_preferencesMenu->Append(COLOR_OPTIONS, _("Grid Colors"), _("Change what colors the grid uses."));
    editMenu->AppendSubMenu(m_preferencesMenu, _("Preferences"));

    auto filterMenu = new wxMenu;
    filterMenu->Append(APPLY_FILTER, _("Apply Filter\tCTRL+SHIFT+Enter"));
    filterMenu->Append(DEFAULT_FILTER, _("Default Filter\tCTRL+d"));
    filterMenu->Append(ADV_FILTER, _("Advanced Filter\tCTRL+SHIFT+f"));
    filterMenu->Append(ADV_SORT, _("Advanced Sort\tCTRL+SHIFT+s"));
    filterMenu->Append(MAKE_DEFAULT_FILTER, _("Make Default Filter"),
                       _("Will make the currently selected filter the default"));

    auto helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    menuBar->Append(m_fileMenu, _("&File"));
    menuBar->Append(editMenu, _("&Edit"));
    menuBar->Append(filterMenu, _("Fi&lter"));
    menuBar->Append(helpMenu, _("&Help"));

    //
    //noteBook
    //
    auto mainPanel = new wxPanel(this, wxID_ANY);
    auto mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    auto notebook = new wxNotebook(mainPanel, wxID_ANY);
    m_topBar = new TopBar(this, this, wxID_ANY, m_connection.get());
    m_dataPanel = new DataPanel(
        notebook, this, wxID_ANY, m_connection.get(), m_settings.get(),
        m_topBar);
    m_analysisPanel = new AnalysisPanel(
        notebook, this, wxID_ANY, m_connection.get(), m_topBar);
    m_dataPanel->AttachTopBar();
    notebook->AddPage(m_dataPanel, _("Data"));
    notebook->AddPage(m_analysisPanel, _("Analysis"));
    mainPanelSizer->Add(notebook, wxSizerFlags(1).Expand());
    mainPanel->SetSizerAndFit(mainPanelSizer);
}

bool MainFrame::UnsavedChangesExist() { return m_unsavedChanges; }

void MainFrame::SetUnsavedChanges(bool unsavedChanges)
{
    if(unsavedChanges && !m_unsavedChanges)
        SetTitle("*" + GetTitle());
    else if(m_unsavedChanges && !unsavedChanges)
        SetTitle(GetTitle().Mid(1));
    m_unsavedChanges = unsavedChanges;
}

void MainFrame::UpdateStats()
{
    m_dataPanel->ApplyFilter();
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
    if(UnsavedChangesExist() && event.CanVeto()){
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
        SetUnsavedChanges(false);
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

void MainFrame::OnAddRow(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->AddRow();
}

void MainFrame::OnDeleteRows(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->DeleteRows();
}

void MainFrame::OnAliasTitle(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->AliasTitle();
}

void MainFrame::OnEditTags(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->EditTags();
}

void MainFrame::OnApplyFilter(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->ApplyFilter();
}

void MainFrame::OnDefaultFilter(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->DefaultFilter();
}

void MainFrame::OnAdvFilter(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->AdvFilter();
}

void MainFrame::OnAdvSort(wxCommandEvent& WXUNUSED(event))
{
    m_topBar->AdvSort();
}

void MainFrame::OnMakeDefaultFilter(wxCommandEvent& WXUNUSED(event))
{
    try {
        auto quickFilter = m_dataPanel->GetQuickFilter();
        auto curFilter = quickFilter->GetSelectedFilterName();
        if (curFilter == "") {
            wxMessageBox("No filter selected");
            return;
        }
        SetUnsavedChanges(true);
        quickFilter->SetDefaultFilter(std::string(curFilter.utf8_str()));
        wxMessageBox("The default filter is now \"" + curFilter + "\"");
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(e.what());
        Close(true);
    }
}

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
    if(!(UnsavedChangesExist() &&
         SaveChangesPopup() == wxID_CANCEL)){
        if (!CreateMemoryDb()) {
            return;
        }
        m_fileMenu->Check(DEFAULT_DB, false);
        m_fileMenu->Enable(DEFAULT_DB, false);
        Reset(m_connection.get());
    }
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    if (!(UnsavedChangesExist() &&
          SaveChangesPopup() == wxID_CANCEL)){
        wxString dir = wxStandardPaths::Get().GetDocumentsDir();
        wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString,
                         dir, "DB files (*.db)|*.db", wxFD_OPEN);
        int status;
        do {
            status = dlg.ShowModal();
            if(status == wxID_OK){
                if (!OpenDb(dlg.GetPath())) {
                    return;
                }
                break;
            }
            else break;
        } while(true);
        if (status == wxID_OK) {
            if (m_dbFile.compare(m_settings->defaultDb)) {
                DoDefaultDbPopup();
            } else {
                m_fileMenu->Enable(DEFAULT_DB, true);
                m_fileMenu->Check(DEFAULT_DB, true);
            }
            Reset(m_connection.get());
        }
    }
}

void MainFrame::OnImportMAL(wxCommandEvent& WXUNUSED(event))
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
            SetUnsavedChanges(true);
            m_dataPanel->UpdateCellColorInfo();
            m_dataPanel->RefreshFilter();
        }
    }
}

bool MainFrame::CreateMemoryDb()
{
    m_connection = std::make_unique<cppw::Sqlite3Connection>(":memory:");
    m_dbInMemory = true;
    m_dbFile = "";
    SetDbFlags(m_connection.get());
    wxString createStr;
    readFileIntoString(createStr, "create.sql", this);
    try {
        m_connection->ExecuteQuery(std::string(createStr.utf8_str()));
        UpdateDb(0);
        MakeTempSeriesTable();
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(std::string("Error: ") + e.what());
        Destroy();
        return false;
    }
    return true;
}

bool MainFrame::OpenDb(const wxString& file)
{
    m_connection = std::make_unique<cppw::Sqlite3Connection>(
        std::string(file.utf8_str()));
    m_dbFile = file;
    SetDbFlags(m_connection.get());
    try {
        if (UpdateDb(GetDbVersion())) {
            DbUpdateNotify();
        }
        MakeTempSeriesTable();
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(std::string("Error: ") + e.what());
        Destroy();
        return false;
    }
    return true;
}

void MainFrame::OnTabChange(wxBookCtrlEvent& event)
{
    wxWindowUpdateLocker lock(this);
    if(event.GetSelection() == 1){
        try{
            std::pair<int, bool> buttonRules[] = {
                {TopBar::id_apply_filter_btn, true},
                {TopBar::id_default_filter_btn, true},
                {TopBar::id_adv_filter_btn, true},
                {TopBar::id_adv_sort_btn, false},
                {TopBar::id_add_row_btn, false},
                {TopBar::id_delete_row_btn, false},
                {TopBar::id_alias_title_btn, false},
                {TopBar::id_edit_tags_btn, false}
            };
            for (auto& rule : buttonRules) {
                m_topBar->ShowButton(rule.first, rule.second);
            }
            m_dataPanel->DetachTopBar();
            m_analysisPanel->AttachTopBar();
            m_analysisPanel->ResetStats();
            m_analysisPanel->Layout();
        } catch(const cppw::Sqlite3Exception& e){
            wxMessageBox(e.what());
            Close();
        }
    } else {
        std::pair<int, bool> buttonRules[] = {
            {TopBar::id_apply_filter_btn, true},
            {TopBar::id_default_filter_btn, true},
            {TopBar::id_adv_filter_btn, true},
            {TopBar::id_adv_sort_btn, true},
            {TopBar::id_add_row_btn, true},
            {TopBar::id_delete_row_btn, true},
            {TopBar::id_alias_title_btn, true},
            {TopBar::id_edit_tags_btn, true}
        };
        for (auto& rule : buttonRules) {
            m_topBar->ShowButton(rule.first, rule.second);
        }
        m_analysisPanel->DetachTopBar();
        m_dataPanel->AttachTopBar();
        m_dataPanel->ApplyFilter();
        m_dataPanel->Layout();
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

int MainFrame::GetDbVersion()
{
    //-1 and 0 logic are because I was a bad boy and was not using versioned DBs
    auto neg_one_stmt = m_connection->PrepareStatement(
        "select name from sqlite_master where type='table' and name='Tag'");
    auto neg_one_results = neg_one_stmt->GetResults();
    if (!neg_one_results->NextRow()) {
        return -1;
    }
    auto zero_stmt = m_connection->PrepareStatement(
        "select name from sqlite_master where type='table' and name='Version'");
    auto zero_results = zero_stmt->GetResults();
    if (!zero_results->NextRow()) {
        return 0;
    }
    auto version_stmt = m_connection->PrepareStatement(
        "select version from Version");
    auto version_results = version_stmt->GetResults();
    version_results->NextRow();
    return version_results->GetInt(0);
}

bool MainFrame::UpdateDb(int version)
{
    bool updated = false;
    if (version == -1) {
        UpdateNegOneDb();
        updated = true;
        version++;
    }
    for (int i = version; i < current_db_version; ++i) {
        wxString fileStr;
        readFileIntoString(
            fileStr,
            std::string("create_p") + std::to_string(i + 1) + ".sql",
            this);
        m_connection->ExecuteQuery(std::string(fileStr.utf8_str()));
        updated = true;
    }
    return updated;
}

void MainFrame::DbUpdateNotify() {
    SetUnsavedChanges(true);
    wxMessageBox("Your database has been updated to the newest version.\n"
                 "If you do not wish to have your database structure modified, please close the application and do not save.\n"
                 "For details about the changes, please consult the changelog.");
}

void MainFrame::UpdateNegOneDb()
{
    m_connection->ExecuteQuery(
        "create table Tag("
        "idTag integer primary key autoincrement, "
        "idSeries integer not null, "
        "tag text not null, "
        "val text not null default '',"
        "unique (idSeries, tag, val), "
        "foreign key (idSeries) references Series (idSeries) on delete cascade on update cascade)");
    auto select_stmt = m_connection->PrepareStatement("select idSeries, studio from Series");
    auto select_results = select_stmt->GetResults();
    auto insert_stmt = m_connection->PrepareStatement(
        "insert into Tag (idSeries, tag, val) values (?, ?, ?)");
    while (select_results->NextRow()) {
        auto idSeries = select_results->GetInt(0);
        auto studio = select_results->GetString(1);
        std::replace(studio.begin(), studio.end(), '/', ',');
        std::replace(studio.begin(), studio.end(), '\\', ',');
        std::replace(studio.begin(), studio.end(), '|', ',');
        size_t begin = 0;
        size_t end = studio.find(',', begin);
        auto pos = end;
        do {
            std::string tok;
            if (end != std::string::npos) {
                tok = studio.substr(begin, end-begin);
            } else {
                tok = studio.substr(begin, studio.size()-begin);
            }
            auto first_not_space = tok.find_first_not_of(" \t");
            auto last_not_space = tok.find_last_not_of(" \t");
            if (first_not_space == std::string::npos) {
                tok = "";
            } else if (last_not_space != std::string::npos) {
                tok = tok.substr(first_not_space, last_not_space-first_not_space+1);
            } else {
                tok = tok.substr(first_not_space, tok.size() - first_not_space);
            }
            if (!tok.empty()) {
                insert_stmt->Bind(1, idSeries);
                insert_stmt->Bind(2, "Studio");
                insert_stmt->Bind(3, tok);
                auto insert_results = insert_stmt->GetResults();
                try {
                    insert_results->NextRow();
                } catch (cppw::Sqlite3Exception e) {
                    if (e.GetErrorCode() != SQLITE_CONSTRAINT) {
                        throw;
                    }
                }
                insert_stmt->Reset();
                insert_stmt->ClearBindings();
            }
            pos = end;
            begin = end+1;
            end = studio.find(',', begin);
        } while (pos != std::string::npos);
    }
}

void MainFrame::OnColorOptions(wxCommandEvent& WXUNUSED(event))
{
    ColorOptionsDlg dlg(m_settings.get(), m_dataPanel, this);
    dlg.ShowModal();
}

void MainFrame::Reset(cppw::Sqlite3Connection* connection)
{
    m_topBar->Reset(connection);
    m_dataPanel->ResetPanel(connection);
    m_analysisPanel->ResetConnection(connection);
    SetUnsavedChanges(false);
}

void MainFrame::MakeTempSeriesTable()
{
    wxString temp;
    readFileIntoString(temp, "basicSelect.sql", this);
    std::string statementStr = std::string(
        ("create temp table tempSeries as " + temp + " where 1<>1").utf8_str());
    auto stmt = m_connection->PrepareStatement(statementStr);
    stmt->Bind(1, "");
    auto result = stmt->GetResults();
    result->NextRow();
}
