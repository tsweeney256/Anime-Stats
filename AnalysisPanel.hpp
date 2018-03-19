#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"
#include "StatsPanel.hpp"

class wxSizer;
class MainFrame;
class AnalysisBox;
class TopBar;

class AnalysisPanel : public StatsPanel
{
public:
    AnalysisPanel(
        wxWindow* parent, MainFrame* top, wxWindowID id,
        cppw::Sqlite3Connection* connection, TopBar* topBar);
    void UpdateInfo();
private:
    void dummy() override {}

    std::vector<wxString> GetDbLabels(
        cppw::Sqlite3Connection*connection , std::string statementStr);
    std::vector<AnalysisBox*> m_statBoxes;
};
