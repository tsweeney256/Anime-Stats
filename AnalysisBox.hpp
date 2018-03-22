#pragma once

#include <wx/panel.h>
#include "cppw/Sqlite3.hpp"

class wxSizer;
class wxCheckBox;
class MainFrame;

class AnalysisBox : public wxPanel
{
public:
    AnalysisBox(
        wxWindow* parent, MainFrame* top, wxWindowID id, wxString boxLabel,
        cppw::Sqlite3Result* result, int startCol, int endCol,
        bool rightAlign = true);
private:
    MainFrame* m_top;
    wxSizer* m_mainSizer;
};
