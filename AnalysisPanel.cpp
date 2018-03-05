#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include "MainFrame.hpp"
#include "AnalysisPanel.hpp"
#include "AnalysisTextBox.hpp"
#include "SqlStrings.hpp"

AnalysisPanel::AnalysisPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, MainFrame* top)
    : wxScrolledWindow(parent) 
{
    SetScrollRate(10, 10);
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);

    try{
        m_statBoxes.push_back(new AnalysisTextBox(connection, this, top, "Time Spent on Anime", {"Days: ", "Formatted: "},
                                                  SqlStrings::timeWatchedSql, SqlStrings::timeRewatchedSql, "Including Rewatches"));
        m_mainSizer->Add(m_statBoxes.back(), wxSizerFlags(0).Border(wxALL));
    } catch(const cppw::Sqlite3Exception& e){
        wxMessageBox(e.what());
        top->Close(true);
    }
    
    SetSizerAndFit(m_mainSizer);
}

void AnalysisPanel::UpdateInfo()
{
    for(auto statBox : m_statBoxes){
        statBox->UpdateInfo();
    }
    m_mainSizer->Layout();
}