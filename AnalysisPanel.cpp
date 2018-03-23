/*Anime Stats
  Copyright (C) 2018 Thomas Sweeney
  This file is part of Anime Stats.
  Anime Stats is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Anime Stats is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <memory>
#include <tuple>
#include <wx/combobox.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <fmt/format.h>
#include "AnalysisBox.hpp"
#include "GroupStatsDlg.hpp"
#include "MainFrame.hpp"
#include "QuickFilter.hpp"
#include "SqlStrings.hpp"
#include "TopBar.hpp"
#include "AnalysisPanel.hpp"

using namespace fmt::literals;

wxBEGIN_EVENT_TABLE(AnalysisPanel, StatsPanel)
    EVT_COMMAND(TopBar::id_apply_filter_btn, TopBarButtonEvent,
                AnalysisPanel::OnApplyFilter)
    EVT_COMMAND(TopBar::id_default_filter_btn, TopBarButtonEvent,
                AnalysisPanel::OnDefaultFilter)
    EVT_COMMAND(TopBar::id_group_stats_btn, TopBarButtonEvent,
                AnalysisPanel::OnGroupStats)
wxEND_EVENT_TABLE()

AnalysisPanel::AnalysisPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
                             cppw::Sqlite3Connection* connection, TopBar* topBar)
    : StatsPanel(parent, top, id, connection, topBar)
{
    m_dlg = new GroupStatsDlg(this, m_top, wxID_ANY, m_connection);
    m_statSizer = new wxBoxSizer(wxVERTICAL);
    m_mainSizer->Add(m_statSizer, wxSizerFlags(0));
}

AnalysisPanel::~AnalysisPanel()
{
    m_dlg->Destroy();
}

void AnalysisPanel::ResetConnection(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
    m_groupCol = "constant";
    m_fromTable = "tempSeries";
    m_havingExpr = "";
    m_orderCol = "constant";
    m_orderDirection = "asc";
}

void AnalysisPanel::ResetStats()
{
    auto tempTable = m_topBar->GetQuickFilter()->GetAnimeData(true, false, true);
    tempTable->NextRow();

    m_statSizer->Clear(true);
    auto str = fmt::format(
        SqlStrings::bigUglyAmalgamation,
        "group_col"_a = m_groupCol,
        "from_table"_a = m_fromTable,
        "having_expr"_a = m_havingExpr,
        "order_col"_a = m_orderCol,
        "order_direction"_a = m_orderDirection);
    try {
        auto stmt = m_connection->PrepareStatement(str);
        auto results = stmt->GetResults();
        std::vector<std::tuple<std::string /*box name*/,
                               bool /*right align*/,
                               int /*start col*/,
                               int /*end col*/>> boxSettings
        {
            {"Time Spent on Anime", false, 1, 5},
            {"Ratings", true, 5, 9},
            {"Watched Status Count", true, 9, 14},
            {"Release Type Count", true, 14, 19}
        };

        while (results->NextRow()) {
            auto statRow = new wxBoxSizer(wxHORIZONTAL);
            if (m_groupCol != "constant") {
                std::string text =
                    results->IsNull(0) || results->GetString(0) == ""
                    ? "Null" : results->GetString(0);
                auto label = new wxStaticText(
                    this, wxID_ANY,
                    m_groupCol + " " + text + ":");
                label->SetFont(
                    wxFont(GetFont().GetPointSize(), wxFONTFAMILY_TELETYPE,
                           wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
                m_statSizer->Add(label, wxSizerFlags(0).Border(wxLEFT | wxTOP));
            }
            for (auto& settings : boxSettings)
            {
                auto boxTitle = std::get<0>(settings);
                auto alignRight = std::get<1>(settings);
                auto startCol = std::get<2>(settings);
                auto endCol = std::get<3>(settings);
                auto dataBox = new AnalysisBox(
                    this, m_top, wxID_ANY, boxTitle, results.get(),
                    startCol, endCol, alignRight);
                statRow->Add(dataBox, wxSizerFlags(0).Border(wxALL).Expand());
            }
            m_statSizer->Add(statRow, wxSizerFlags(0));
        }
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(e.what());
        m_top->Close(true);
    }

    m_mainSizer->SetSizeHints(this);
    Layout();
}

void AnalysisPanel::OnApplyFilter(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

void AnalysisPanel::OnDefaultFilter(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

void AnalysisPanel::OnGroupStats(wxCommandEvent& WXUNUSED(event))
{
    if (m_dlg->ShowModal() == wxID_OK) {
        auto info = m_dlg->GetGroupStatsInfo();
        m_groupCol = info.groupCol;
        m_fromTable = info.fromTable;
        m_havingExpr = info.havingExpr;
        m_orderCol = info.orderCol;
        m_orderDirection = info.orderDirection;
        ResetStats();
    }
}

void AnalysisPanel::ApplyFilter()
{
    ResetStats();
}
