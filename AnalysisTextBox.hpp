#pragma once

#include <wx/panel.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <vector>
#include <string>
#include <memory>
#include "cppw/Sqlite3.hpp"

class wxSizer;

class AnalysisTextBox : public wxPanel
{
public:
    AnalysisTextBox(cppw::Sqlite3Connection* connection, wxWindow* parent, wxString boxLabel,
                    std::vector<wxString> outputLabels, const std::string& sqlScript, const std::string& altSqlScript = "",
                    wxString checkBoxLabel = "", int numBinds = 0,
                    const std::vector<wxString>& comboVals = std::vector<wxString>());
    void UpdateInfo();

private:
    enum{
        checkbox = wxID_HIGHEST + 1
    };

    void OnCheckBox(wxCommandEvent& event);
    
    cppw::Sqlite3Connection* m_connection;
    std::unique_ptr<cppw::Sqlite3Statement> m_statement;
    std::unique_ptr<cppw::Sqlite3Statement> m_altStatement;
    std::vector<wxString> m_outputLabels;
    std::vector<wxStaticText*> m_staticText;
    wxSizer* m_mainSizer;
    int m_numBinds;
};
