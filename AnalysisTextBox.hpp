#pragma once

#include <wx/panel.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <vector>
#include <string>
#include <memory>
#include "cppw/Sqlite3.hpp"

class wxSizer;
class wxCheckBox;
class MainFrame;

class AnalysisTextBox : public wxPanel
{
public:
    AnalysisTextBox(
        cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top,
        wxString boxLabel, std::vector<wxString> outputLabels,
        const std::string& sqlScript, const std::string& altSqlScript = "",
        wxString checkBoxLabel = "");
    void UpdateInfo();

private:
    enum{
        checkbox = wxID_HIGHEST + 1
    };

    void OnCheckBox(wxCommandEvent& event);

    MainFrame* m_top;
    std::unique_ptr<cppw::Sqlite3Statement> m_statement;
    std::unique_ptr<cppw::Sqlite3Statement> m_altStatement;
    std::vector<wxString> m_outputLabels;
    std::vector<wxStaticText*> m_staticText;
    wxCheckBox* m_checkBox;
    wxSizer* m_mainSizer;

    DECLARE_EVENT_TABLE()
};
