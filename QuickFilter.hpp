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

#include <memory>
#include <vector>
#include <wx/panel.h>
#include "FilterStructs.hpp"
#include "SortStruct.hpp"

class wxTextCtrl;
class wxComboBox;
class wxButton;

namespace cppw
{
    class Sqlite3Connection;
    class Sqlite3Result;
    class Sqlite3Statement;
}

class DbFilter;
class MainFrame;

class QuickFilter : public wxPanel
{
public:
    QuickFilter(wxWindow* parent, MainFrame* top, wxWindowID id,
                cppw::Sqlite3Connection* connection);
    void Reset(cppw::Sqlite3Connection* connection);
    ConstFilter GetFilter() const;
    const std::vector<colSort>* GetSort() const;
    void LoadDefaultFilter();
    void SetFilter(std::unique_ptr<BasicFilterInfo> bfi,
                   std::unique_ptr<AdvFilterInfo> afi);
    void SetSort(std::vector<colSort> sortingRules);
    void SetDefaultFilter(std::string name);
    wxString GetSelectedFilterName() const;
    cppw::Sqlite3Result* GetAnimeData(bool filtered, bool sorted);

private:
    void OnProcessEnter(wxCommandEvent& event);
    void OnTitleText(wxCommandEvent& envet);
    void OnComboBoxChange(wxCommandEvent& event);
    void OnQuickFilterNew(wxCommandEvent& event);
    void OnQuickFilterOverwrite(wxCommandEvent& event);
    void OnQuickFilterDelete(wxCommandEvent& event);

    void AppendStatusStr(
        std::stringstream& statusStr, std::string toAppend, bool& firstStatus);
    std::string CreateSortStr();

    std::unique_ptr<cppw::Sqlite3Result> m_animeStatsResults;
    std::unique_ptr<cppw::Sqlite3Statement> m_animeStatsStmt;
    std::string m_basicSelectString;
    cppw::Sqlite3Connection* m_connection;
    std::unique_ptr<DbFilter> m_dbFilter;
    wxTextCtrl* m_quickFilterTitle;
    wxComboBox* m_quickFilterSelect;
    MainFrame* m_top;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_EVENT(QuickFilterProcessEnterEvent, wxCommandEvent);
