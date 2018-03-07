#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/msgdlg.h>
#include "MainFrame.hpp"
#include "AnalysisBox.hpp"

BEGIN_EVENT_TABLE(AnalysisTextBox, AnalysisBox)
    EVT_CHECKBOX(wxID_ANY, AnalysisTextBox::OnCheckBox)
END_EVENT_TABLE()

AnalysisBox::AnalysisBox(
    wxWindow* parent, MainFrame* top, wxString boxLabel,
    cppw::Sqlite3Connection* connection, const std::string& sqlScript)
    : wxPanel(parent), m_top(top),
      m_statement(connection->PrepareStatement(sqlScript))

{
    m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, boxLabel);
    SetSizer(m_mainSizer);
}

AnalysisTextBox::AnalysisTextBox(
    wxWindow* parent, MainFrame* top, cppw::Sqlite3Connection* connection,
    const std::string& sqlScript, wxString boxLabel,
    std::vector<wxString> outputLabels, const std::string& altSqlScript,
    wxString checkBoxLabel)
    : AnalysisBox(parent, top, boxLabel, connection, sqlScript),
      m_altStatement(altSqlScript.empty() ? nullptr :
                     connection->PrepareStatement(altSqlScript)),
      m_outputLabels(outputLabels)
{
    auto controlSizer = new wxBoxSizer(wxHORIZONTAL);
    auto textSizer = new wxBoxSizer(wxVERTICAL);

    if (!altSqlScript.empty()) {
        m_checkBox = new wxCheckBox(
            this, AnalysisTextBox::checkbox, checkBoxLabel);
        controlSizer->Add(m_checkBox, wxSizerFlags(1).Border(wxALL));
    }

    auto result = m_statement->GetResults();
    for (int i = 0; result->NextRow(); ++i) {
        m_staticText.push_back(new wxStaticText(this, wxID_ANY, ""));
        textSizer->Add(m_staticText.back(), wxSizerFlags(0).Border(wxALL));
    }

    m_mainSizer->Add(controlSizer, wxSizerFlags(0).Border(wxALL));
    m_mainSizer->Add(textSizer, wxSizerFlags(0).Border(wxALL));
    m_mainSizer->SetSizeHints(this);
}

void AnalysisTextBox::UpdateInfo()
{
    std::unique_ptr<cppw::Sqlite3Result> result;
    if (m_checkBox && m_checkBox->GetValue()) {
        m_altStatement->Reset();
        result = m_altStatement->GetResults();
    } else {
        m_statement->Reset();
        result = m_statement->GetResults();
    }
    for (int i = 0; result->NextRow(); ++i) {
        m_staticText[i]->SetLabel(
            m_outputLabels[i] + ": " +
            wxString::FromUTF8(result->GetString(0).c_str()));
    }
    m_mainSizer->SetSizeHints(this);
}

void AnalysisTextBox::OnCheckBox(wxCommandEvent& WXUNUSED(event))
{
    try {
        UpdateInfo();
    } catch(const cppw::Sqlite3Exception& e){
        wxMessageBox(e.what());
        m_top->Close();
    }
}
