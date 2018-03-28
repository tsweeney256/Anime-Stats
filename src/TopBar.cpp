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

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include "cppw/Sqlite3.hpp"
#include "AdvFilterFrame.hpp"
#include "AdvSortFrame.hpp"
#include "QuickFilter.hpp"
#include "TopBar.hpp"

wxBEGIN_EVENT_TABLE(TopBar, wxScrolledWindow)
    EVT_WINDOW_DESTROY(TopBar::OnAdvrFrameDestruction)
    EVT_BUTTON(wxID_ANY, TopBar::OnButton)
    EVT_COMMAND(wxID_ANY, QuickFilterProcessEnterEvent, TopBar::OnTextEnter)
wxEND_EVENT_TABLE()

enum {
    id_adv_filter_frame = wxID_HIGHEST + 1,
    id_adv_sort_frame
};

TopBar::TopBar(wxWindow* parent, MainFrame* top, wxWindowID id,
               cppw::Sqlite3Connection* connection)
    : wxScrolledWindow(parent, id), m_connection(connection), m_top(top)
{
    SetScrollRate(10, 10);
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    m_quickFilter = new QuickFilter(this, m_top, wxID_ANY, m_connection);
    wxString buttonNames[] = {
        "Apply Filter",
        "Default Filter",
        "Adv. Filter",
        "Adv. Sort",
        "Add Row",
        "Delete Rows",
        "Alias Title",
        "Edit Tags",
        "Group Stats"
    };
    for (int i = 0; i < num_buttons; ++i) {
        m_buttons[i] = new wxButton(this, i + wxID_HIGHEST + 1, buttonNames[i]);
        buttonSizer->Add(m_buttons[i], wxSizerFlags(1).Expand());
    }
    m_buttons[num_buttons-1]->Show(false);
    mainSizer->Add(
        m_quickFilter, wxSizerFlags(0).Expand().Border(wxTOP | wxLEFT));
    mainSizer->Add(
        buttonSizer, wxSizerFlags(0).Expand().Border(wxALL ^ wxRIGHT ));
    SetSizerAndFit(mainSizer);
}

void TopBar::Reset(cppw::Sqlite3Connection* connection)
{
    //maybe reset buttons visibility?
    m_connection = connection;
    m_quickFilter->Reset(connection);
}

void TopBar::ShowButton(int buttonId, bool enable)
{
    m_buttons[AdjustButtonId(buttonId)]->Show(enable);
}

QuickFilter* TopBar::GetQuickFilter()
{
    const auto* cThis = this;
    return const_cast<QuickFilter*>(cThis->GetQuickFilter());
}

const QuickFilter* TopBar::GetQuickFilter() const
{
    return m_quickFilter;
}

void TopBar::ApplyFilter(bool useQuickCombo)
{
    if (useQuickCombo) {
    m_quickFilter->SetFilter(
            std::string(m_quickFilter->GetSelectedFilterName().utf8_str()));
    }
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_apply_filter_btn);
    QueueEvent(applyEvent);
}

void TopBar::DefaultFilter()
{
    m_quickFilter->LoadDefaultFilter();
    auto applyEvent = new wxCommandEvent(
        TopBarButtonEvent, id_default_filter_btn);
    QueueEvent(applyEvent);
}

void TopBar::AdvFilter()
{
    //non-modal
    auto frame = new AdvFilterFrame(this, id_adv_filter_frame,
                                    "Advanced Filtering", wxDefaultPosition,
                                    wxDefaultSize);
    frame->Show(true);
    m_buttons[AdjustButtonId(id_adv_filter_btn)]->Disable();
}

void TopBar::AdvSort()
{
    //non-modal
    auto frame = new AdvSortFrame(
        m_quickFilter, m_top, id_adv_sort_frame, m_connection);
    frame->Show(true);
    m_buttons[AdjustButtonId(id_adv_sort_btn)]->Disable();
}

void TopBar::AddRow()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_add_row_btn);
    QueueEvent(applyEvent);
}

void TopBar::DeleteRows()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_delete_row_btn);
    QueueEvent(applyEvent);
}

void TopBar::AliasTitle()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_alias_title_btn);
    QueueEvent(applyEvent);
}

void TopBar::EditTags()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_edit_tags_btn);
    QueueEvent(applyEvent);
}

void TopBar::GroupStats()
{
    auto applyEvent = new wxCommandEvent(TopBarButtonEvent, id_group_stats_btn);
    QueueEvent(applyEvent);
}

void TopBar::OnAdvrFrameDestruction(wxWindowDestroyEvent& event)
{
    if(event.GetId() == id_adv_filter_frame)
        m_buttons[AdjustButtonId(id_adv_filter_btn)]->Enable();
    else if(event.GetId() == id_adv_sort_frame)
        m_buttons[AdjustButtonId(id_adv_sort_btn)]->Enable();
    auto newEvent = new wxCommandEvent(TopBarButtonEvent, id_apply_filter_btn);
    QueueEvent(newEvent);
}

void TopBar::OnButton(wxCommandEvent& event)
{
    switch (event.GetId()) {
    case id_apply_filter_btn:
        ApplyFilter();
        break;
    case id_default_filter_btn:
        DefaultFilter();
        break;
    case id_adv_filter_btn:
        AdvFilter();
        break;
    case id_adv_sort_btn:
        AdvSort();
        break;
    default:
        auto clone = event.Clone();
        clone->SetEventType(TopBarButtonEvent);
        QueueEvent(clone);
        break;
    }
}

void TopBar::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

int TopBar::AdjustButtonId(int id)
{
    return id - wxID_HIGHEST - 1;
}

wxDEFINE_EVENT(TopBarButtonEvent, wxCommandEvent);
