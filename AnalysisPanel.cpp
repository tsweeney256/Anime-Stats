#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <fmt/format.h>
#include "MainFrame.hpp"
#include "AnalysisPanel.hpp"
#include "AnalysisBox.hpp"
#include "SqlStrings.hpp"

AnalysisPanel::AnalysisPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
                             cppw::Sqlite3Connection* connection, TopBar* topBar)
    : StatsPanel(parent, top, id, connection, topBar)
{
    ResetStats();
}

void AnalysisPanel::ResetConnection(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
}

void AnalysisPanel::ResetStats()
{
    if (m_statSizer) {
        m_statSizer->Clear();
    }
    m_statSizer = new wxBoxSizer(wxHORIZONTAL);
    auto statSizerFlags = wxSizerFlags(0).Border(wxALL).Expand();

    auto stmt = m_connection->PrepareStatement(SqlStrings::timeWatchedSql);
    auto result = stmt->GetResults();
    m_statSizer->Add(
        new AnalysisBox(this, m_top, wxID_ANY, result.get(),
                        "Time Spent on Anime", false), statSizerFlags);

    stmt = m_connection->PrepareStatement(SqlStrings::distribution);
    result = stmt->GetResults();
    m_statSizer->Add(
        new AnalysisBox(this, m_top, wxID_ANY, result.get(), "Ratings"),
        statSizerFlags);

    stmt = m_connection->PrepareStatement(SqlStrings::countWatchedStatus);
    result = stmt->GetResults();
    m_statSizer->Add(
        new AnalysisBox(
            this, m_top, wxID_ANY, result.get(), "Watched Status Count"),
        statSizerFlags);

    stmt = m_connection->PrepareStatement(SqlStrings::countReleaseType);
    result = stmt->GetResults();
    m_statSizer->Add(
        new AnalysisBox(
            this, m_top, wxID_ANY, result.get(), "Release Type Count"),
        statSizerFlags);

    m_mainSizer->Add(m_statSizer, wxSizerFlags(0));
    m_mainSizer->SetSizeHints(this);
}
