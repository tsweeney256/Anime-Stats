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

class AnalysisBox : public wxPanel
{
public:
    AnalysisBox(wxWindow* parent, MainFrame* top, wxString boxLabel,
                cppw::Sqlite3Connection* connection,
                const std::string& sqlScript);
    virtual ~AnalysisBox() = default;
    virtual void UpdateInfo() = 0;

protected:
    MainFrame* m_top;
    std::unique_ptr<cppw::Sqlite3Statement> m_statement;
    wxSizer* m_mainSizer;
};

class AnalysisTextBox : public AnalysisBox
{
public:
    AnalysisTextBox(
        wxWindow* parent, MainFrame* top,
        cppw::Sqlite3Connection* connection, const std::string& sqlScript,
        wxString boxLabel, std::vector<wxString> outputLabels,
        bool rightAlign = true, const std::string& altSqlScript = "",
        wxString checkBoxLabel = "");
    void UpdateInfo() override;

private:
    enum{
        checkbox = wxID_HIGHEST + 1
    };

    void OnCheckBox(wxCommandEvent& event);

    std::unique_ptr<cppw::Sqlite3Statement> m_altStatement;
    std::vector<wxString> m_outputLabels;
    std::vector<wxStaticText*> m_staticText;
    wxCheckBox* m_checkBox = nullptr;

    DECLARE_EVENT_TABLE()
};
