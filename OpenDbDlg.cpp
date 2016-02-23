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
