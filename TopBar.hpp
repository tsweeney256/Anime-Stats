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

#include <array>
#include <wx/scrolwin.h>

class wxButton;

namespace cppw
{
    class Sqlite3Connection;
}

class MainFrame;
class QuickFilter;

class TopBar : public wxScrolledWindow
{
public:
    enum {
        id_apply_filter_btn = wxID_HIGHEST + 1,
        id_default_filter_btn,
        id_adv_filter_btn,
        id_adv_sort_btn,
        id_add_row_btn,
        id_delete_row_btn,
        id_alias_title_btn,
        id_edit_tags_btn,
        dummy, // always keep this above num_buttons
        num_buttons = dummy - wxID_HIGHEST - 1
    };

    TopBar(wxWindow* parent, MainFrame* top, wxWindowID id,
           cppw::Sqlite3Connection* connection);

    void Reset(cppw::Sqlite3Connection* connection);
    void ShowButton(int buttonId, bool enable=true);
    QuickFilter* GetQuickFilter();
    const QuickFilter* GetQuickFilter() const;
    void ApplyFilter();
    void DefaultFilter();
    void AdvFilter();
    void AdvSort();
    void AddRow();
    void DeleteRows();
    void AliasTitle();
    void EditTags();

private:
    void OnAdvrFrameDestruction(wxWindowDestroyEvent& event);
    void OnButton(wxCommandEvent& event);

    int AdjustButtonId(int id);

    std::array<wxButton*, num_buttons> m_buttons;
    cppw::Sqlite3Connection* m_connection;
    QuickFilter* m_quickFilter;
    MainFrame* m_top;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_EVENT(TopBarButtonEvent, wxCommandEvent);
