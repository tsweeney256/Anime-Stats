#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include "MainFrame.hpp"
#include "AnalysisPanel.hpp"
#include "AnalysisBox.hpp"
#include "SqlStrings.hpp"

AnalysisPanel::AnalysisPanel(wxWindow* parent, MainFrame* top,
                             cppw::Sqlite3Connection* connection)
    : wxScrolledWindow(parent)
{
    SetScrollRate(10, 10);
    auto statSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainSizer = new wxBoxSizer(wxVERTICAL);

    try{
        m_statBoxes.push_back(
            new AnalysisTextBox(
                this, top, connection, SqlStrings::timeWatchedSql,
                "Time Spent on Anime", {"Days", "Formatted"}, false,
                SqlStrings::timeRewatchedSql, "Including Rewatches"));

        wxString wxAvgMedianStr(SqlStrings::avgStr +
                                "\nUNION ALL\n" +
                                SqlStrings::medianStr);
        wxAvgMedianStr = wxString::Format(wxAvgMedianStr, "rating", "%");
        std::string avgMedianStr(wxAvgMedianStr.utf8_str());
        m_statBoxes.push_back(
            new AnalysisTextBox(this, top, connection, avgMedianStr, "Ratings",
                                {"Average", "Median"}));

        auto watchedStatusLabels = GetDbLabels(connection,
                                               "select status "
                                               "from WatchedStatus "
                                               "where idWatchedStatus <> 0 "
                                               "order by idWatchedStatus");
        m_statBoxes.push_back(
            new AnalysisTextBox(
                this, top, connection, SqlStrings::countWatchedStatus,
                "Watched Status Count", watchedStatusLabels));

        auto releaseTypeLabels = GetDbLabels(connection,
                                             "select type "
                                             "from ReleaseType "
                                             "where idReleasetype <> 0 "
                                             "order by idReleaseType");
        m_statBoxes.push_back(
            new AnalysisTextBox(
                this, top, connection, SqlStrings::countReleaseType,
                "Release Type Count", releaseTypeLabels));

        for (auto& box : m_statBoxes) {
            statSizer->Add(box, wxSizerFlags(0).Border(wxALL).Expand());
        }
    } catch(const cppw::Sqlite3Exception& e){
        wxMessageBox(e.what());
        top->Close(true);
    }

    m_mainSizer->Add(statSizer, wxSizerFlags(0));
    SetSizerAndFit(m_mainSizer);
}

void AnalysisPanel::UpdateInfo()
{
    for(auto statBox : m_statBoxes){
        statBox->UpdateInfo();
    }
    m_mainSizer->Layout();
}

std::vector<wxString> AnalysisPanel::GetDbLabels(
    cppw::Sqlite3Connection* connection, std::string statementStr)
{
    std::vector<wxString> ret;
    auto stmt = connection->PrepareStatement(statementStr);
    auto results = stmt->GetResults();
    while (results->NextRow()) {
        ret.push_back(results->GetString(0));
    }
    return ret;
}
