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

#include <cstdint>
#include <wx/dialog.h>

namespace cppw { class Sqlite3Connection; }
class wxGrid;
class wxCommandEvent;
class wxGridEvent;

class EditTagDialog : public wxDialog
{
public:
    EditTagDialog() = delete;
    EditTagDialog(wxWindow* parent, wxWindowID id,
                  cppw::Sqlite3Connection* connection,
                  int64_t idSeries, wxString title);
    bool MadeChanges() { return m_madeChanges; }

private:
    void OnAddRowButton(wxCommandEvent& event);
    void OnDeleteRowButton(wxCommandEvent& event);
    void OnGridCellChanging(wxGridEvent& event);

    void HandleInsert(wxString tag);
    void HandleUpdate(int row, int col, wxString val);
    void HandleDelete(int row);

    cppw::Sqlite3Connection* m_connection;
    int64_t m_idSeries;
    wxGrid* m_grid;
    bool m_madeChanges = false;

    DECLARE_EVENT_TABLE()
};
