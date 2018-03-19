#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"
#include "StatsPanel.hpp"

class MainFrame;
class AnalysisBox;
class wxSizer;

class AnalysisPanel : public StatsPanel
{
public:
    AnalysisPanel(
        wxWindow* parent, MainFrame* top, cppw::Sqlite3Connection* connection);
    void UpdateInfo();
private:
    void dummy() override {}

    std::vector<wxString> GetDbLabels(
        cppw::Sqlite3Connection*connection , std::string statementStr);
    std::vector<AnalysisBox*> m_statBoxes;
    wxSizer* m_mainSizer;
};
