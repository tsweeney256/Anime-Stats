/*Anime Stats
  Copyright (C) 2015-2018 Thomas Sweeney
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

#include <string>
#include <utility>
#include <vector>
#include <wx/dialog.h>

class wxComboBox;
class wxRadioButton;
class wxTextCtrl;

namespace cppw
{
    class Sqlite3Connection;
}

class MainFrame;

struct GroupStatsInfo
{
    std::string groupCol;
    std::string fromTable;
    std::string havingExpr;
    std::string orderCol;
    std::string orderDirection;
};

class GroupStatsDlg : public wxDialog
{
public:
    GroupStatsDlg(
        wxWindow* parent, MainFrame* top, wxWindowID id,
        cppw::Sqlite3Connection* connection);
    GroupStatsInfo GetGroupStatsInfo();

private:
    void OnGroupCombo(wxCommandEvent& event);
    void OnAggregateCombo(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);

    void EnableGroupControls(bool enable);
    void EnableHavingControls(bool enable);

    cppw::Sqlite3Connection* m_connection;
    std::vector<std::pair<std::string, std::string>> m_orderOptionsData;
    wxComboBox* m_groupCombo = nullptr;
    wxComboBox* m_havingAggregateCombo = nullptr;
    wxComboBox* m_havingColumnCombo = nullptr;
    wxComboBox* m_havingOperatorCombo = nullptr;
    wxTextCtrl* m_havingValue = nullptr;
    wxComboBox* m_orderCombo = nullptr;
    wxRadioButton* m_orderRadioAsc = nullptr;
    wxRadioButton* m_orderRadioDesc = nullptr;

    wxDECLARE_EVENT_TABLE();
};
