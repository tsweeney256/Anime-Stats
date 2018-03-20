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
    void AttachTopBar();
    void DetachTopBar();
    void ResetConnection(cppw::Sqlite3Connection* connection);
    void ResetStats();

private:
    void OnApplyFilter(wxCommandEvent& event);
    void OnDefaultFilter(wxCommandEvent& event);

    void ApplyFilter();

    wxBoxSizer* m_statSizer = nullptr;

    void dummy() override {}
    wxDECLARE_EVENT_TABLE();
};
