#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"

class MainFrame;
class AnalysisBox;
class wxSizer;

class AnalysisPanel : public wxScrolledWindow
{
public:
    AnalysisPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top);
    void UpdateInfo();
private:
    std::vector<AnalysisBox*> m_statBoxes;
    wxSizer* m_mainSizer;
};
