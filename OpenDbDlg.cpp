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

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/stdpaths.h>
#include <wx/stattext.h>
#include "OpenDbDlg.hpp"

BEGIN_EVENT_TABLE(OpenDbDlg, wxDialog)
    EVT_BUTTON(wxID_APPLY, OpenDbDlg::OnOpen)
    EVT_BUTTON(wxID_OK, OpenDbDlg::OnSave)
    EVT_BUTTON(wxID_CANCEL, OpenDbDlg::OnCancel)
END_EVENT_TABLE()

OpenDbDlg::OpenDbDlg(wxString& file, wxString msg, wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, "Select DB"), m_file(file)
{
    auto mainPanel = new wxPanel(this, wxID_ANY);
    auto text = new wxStaticText(mainPanel,wxID_ANY, msg);
    //using wxID_APPLY and wxID_OK as a hack to get the sizer to order the buttons in an OS specific way
    auto openBtn = new wxButton(mainPanel, wxID_APPLY, "Open");
    auto saveBtn = new wxButton(mainPanel, wxID_OK, "Save");
    auto cancelBtn = new wxButton(mainPanel, wxID_CANCEL, "Cancel");
    auto btnSizer = new wxStdDialogButtonSizer();
    auto panelSizer = new wxBoxSizer(wxVERTICAL);

    btnSizer->AddButton(openBtn);
    btnSizer->AddButton(saveBtn);
    btnSizer->AddButton(cancelBtn);
    btnSizer->Realize();
    auto flags = wxSizerFlags(1).Expand().Border(wxALL);
    panelSizer->Add(text, flags);
    panelSizer->Add(btnSizer, flags);
    mainPanel->SetSizerAndFit(panelSizer);
    SetClientSize(mainPanel->GetClientSize()); //I have no idea why Fit() isn't working
}

void OpenDbDlg::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString dir = wxStandardPaths::Get().GetDocumentsDir();
    wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "DB files (*.db)|*.db", wxFD_OPEN);
    if(dlg.ShowModal() == wxID_OK){
        m_file = dlg.GetPath();
        EndModal(wxID_OPEN);
    }
}

void OpenDbDlg::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString dir = wxStandardPaths::Get().GetDocumentsDir();
    wxFileDialog dlg(this, wxFileSelectorPromptStr, wxEmptyString, dir, "DB files (*.db)|*.db", wxFD_SAVE);
    if(dlg.ShowModal() == wxID_OK){
        m_file = dlg.GetPath();
        EndModal(wxID_SAVE);
    }
}

void OpenDbDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
