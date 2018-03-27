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

#pragma once

#include <vector>
#include <wx/scrolwin.h>
#include "cppw/Sqlite3.hpp"
#include "StatsPanel.hpp"

class wxBoxSizer;
class wxComboBox;
class wxRadioButton;
class wxScrolledWindow;
class wxTextCtrl;
class AnalysisBox;
class GroupStatsDlg;
class MainFrame;
class TopBar;

class AnalysisPanel : public StatsPanel
{
public:
    AnalysisPanel(
        wxWindow* parent, MainFrame* top, wxWindowID id,
        cppw::Sqlite3Connection* connection, TopBar* topBar);
    ~AnalysisPanel();
    void ResetConnection(cppw::Sqlite3Connection* connection);
    void ResetStats();

private:
    void OnApplyFilter(wxCommandEvent& event);
    void OnDefaultFilter(wxCommandEvent& event);
    void OnGroupStats(wxCommandEvent& event);

    void ApplyFilter() override;

    wxScrolledWindow* m_scrollPanel;
    wxBoxSizer* m_scrollSizer;
    GroupStatsDlg* m_dlg;
    std::string m_groupCol = "constant";
    std::string m_fromTable = "tempSeries";
    std::string m_havingExpr = "";
    std::string m_orderCol = "constant";
    std::string m_orderDirection = "asc";
    wxBoxSizer* m_statSizer = nullptr;

    void dummy() override {}
    wxDECLARE_EVENT_TABLE();
};
