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

#include "wx/sizer.h"
#include "cppw/Sqlite3.hpp"
#include "TopBar.hpp"
#include "StatsPanel.hpp"

const char* StatsPanelNotImplementedException::what() const noexcept
{
    return "Method notimplemented";
}

StatsPanel::StatsPanel(wxWindow* parent, MainFrame* top, wxWindowID id,
                       cppw::Sqlite3Connection* connection, TopBar* topBar)
    : wxScrolledWindow(parent, id), m_connection(connection), m_top(top),
      m_topBar(topBar)
{
    SetScrollRate(10, 10);
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_mainSizer);
}

void StatsPanel::AttachTopBar()
{
    if (m_topBar->GetParent() != this) {
        m_mainSizer->Prepend(m_topBar, wxSizerFlags(0));
        m_topBar->Reparent(this);
        Layout();
    }
}

void StatsPanel::DetachTopBar()
{
    m_mainSizer->Detach(m_topBar);
}

void StatsPanel::ApplyFilter()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::DefaultFilter()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::AdvFilter()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::AdvSort()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::AddRow()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::DeleteRows()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::AliasTitle()
{
    throw StatsPanelNotImplementedException();
}

void StatsPanel::EditTags()
{
    throw StatsPanelNotImplementedException();
}
