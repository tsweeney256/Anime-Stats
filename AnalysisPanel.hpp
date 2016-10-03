#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"

class MainFrame;
class AnalysisTextBox;
class wxSizer;

class AnalysisPanel : public wxScrolledWindow
{
public:
    AnalysisPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top);
    void UpdateInfo();
private:
    std::vector<AnalysisTextBox*> m_statBoxes;
    wxSizer* m_mainSizer;
};
