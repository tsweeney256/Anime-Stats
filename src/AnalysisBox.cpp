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

#include <vector>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include "MainFrame.hpp"
#include "AnalysisBox.hpp"

AnalysisBox::AnalysisBox(
    wxWindow* parent, wxWindowID id, wxString boxLabel,
    cppw::Sqlite3Result* result, int startCol, int endCol, bool rightAlign)
    : wxPanel(parent, id)
{
    SetFont(wxFont(GetFont().GetPointSize(), wxFONTFAMILY_TELETYPE,
                   wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    m_mainSizer = new wxStaticBoxSizer(wxVERTICAL, this, boxLabel);
    auto textSizer = new wxBoxSizer(wxVERTICAL);

    for (int i = startCol; i < endCol; ++i) {
        auto rowSizer = new wxBoxSizer(wxHORIZONTAL);
        auto rowItemSizer = new wxBoxSizer(wxVERTICAL);
        auto val = result->IsNull(i) ? "0" : result->GetString(i);
        auto staticText = new wxStaticText(this, wxID_ANY, val);
        rowSizer->Add(
            new wxStaticText(this, wxID_ANY, result->GetColumnName(i) + ":"),
            wxSizerFlags(0).Border(wxLEFT | wxRIGHT | wxBOTTOM));
        auto rowItemSizerFlags = wxSizerFlags(0).Border(wxLEFT | wxRIGHT);
        if (rightAlign) {
            rowItemSizerFlags = rowItemSizerFlags.Right();
        }
        rowItemSizer->Add(staticText, rowItemSizerFlags);
        rowSizer->Add(rowItemSizer, wxSizerFlags(1).Expand());
        textSizer->Add(rowSizer, wxSizerFlags(1).Expand());
    }

    m_mainSizer->Add(textSizer, wxSizerFlags(0).Border(wxALL).Expand());
    SetSizerAndFit(m_mainSizer);
}
