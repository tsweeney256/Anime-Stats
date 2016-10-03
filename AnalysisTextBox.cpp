#include <wx/sizer.h>
#include <wx/checkbox.h>
#include "AnalysisTextBox.hpp"

AnalysisTextBox::AnalysisTextBox(cppw::Sqlite3Connection* connection, wxWindow* parent, wxString boxLabel,
                std::vector<wxString> outputLabels, const std::string& sqlScript, const std::string& altSqlScript,
                wxString checkBoxLabel, int numBinds, const std::vector<wxString>& comboVals)
    : wxPanel(parent), m_connection(connection), m_statement(connection->PrepareStatement(sqlScript)),
      m_outputLabels(outputLabels), m_numBinds(numBinds)
{
    m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, boxLabel);
    auto controlSizer = new wxBoxSizer(wxHORIZONTAL);
    auto textSizer = new wxBoxSizer(wxVERTICAL);

    if(!altSqlScript.empty()){
        auto checkBox = new wxCheckBox(this, AnalysisTextBox::checkbox, checkBoxLabel);
        controlSizer->Add(checkBox, wxSizerFlags(1).Border(wxALL));
    }

    auto result = m_statement->GetResults();
    for(int i = 0; result->NextRow(); ++i){
        m_staticText.push_back(new wxStaticText(this, wxID_ANY, ""));
        m_staticText.back()->Wrap(-1);
        textSizer->Add(m_staticText.back(), wxSizerFlags(0).Border(wxALL));
    }

    m_mainSizer->Add(controlSizer, wxSizerFlags(0).Border(wxALL));
    m_mainSizer->Add(textSizer, wxSizerFlags(0).Border(wxALL));
    SetSizerAndFit(m_mainSizer);
}

void AnalysisTextBox::UpdateInfo()
{
    m_statement->Reset();
    auto result = m_statement->GetResults();
    for(int i = 0; result->NextRow(); ++i){
        m_staticText[i]->SetLabel(m_outputLabels[i] + wxString::FromUTF8(result->GetString(0).c_str()));
    }
    m_mainSizer->SetSizeHints(this);
}

void AnalysisTextBox::OnCheckBox(wxCommandEvent& event)
{

}
