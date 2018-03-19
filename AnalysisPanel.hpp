#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"
#include "StatsPanel.hpp"

class wxBoxSizer;
class MainFrame;
class AnalysisBox;
class TopBar;

class AnalysisPanel : public StatsPanel
{
public:
    AnalysisPanel(
        wxWindow* parent, MainFrame* top, wxWindowID id,
        cppw::Sqlite3Connection* connection, TopBar* topBar);
    void ResetConnection(cppw::Sqlite3Connection* connection);
    void ResetStats();

private:
    void dummy() override {}
    wxBoxSizer* m_statSizer = nullptr;
};
