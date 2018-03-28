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

#include "EditTagDialog.hpp"
#include <cstdint>
#include <string>
#include <algorithm>
#include <wx/panel.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/msgdlg.h>
#include "cppw/Sqlite3.hpp"
#include "Helpers.hpp"

enum {
    ADD_ROW_BUTTON = wxID_HIGHEST+1,
    DELETE_ROW_BUTTON
};

enum {
    IDTAG_COL,
    FIRST_VISIBLE_COL,
    TAG_COL = FIRST_VISIBLE_COL,
    VAL_COL,
    NUM_COLS
};

BEGIN_EVENT_TABLE(EditTagDialog, wxDialog)
    EVT_BUTTON(ADD_ROW_BUTTON, EditTagDialog::OnAddRowButton)
    EVT_BUTTON(DELETE_ROW_BUTTON, EditTagDialog::OnDeleteRowButton)
    EVT_GRID_CELL_CHANGING(EditTagDialog::OnGridCellChanging)
    EVT_GRID_SELECT_CELL(EditTagDialog::OnGridCellSelect)
END_EVENT_TABLE()

EditTagDialog::EditTagDialog(wxWindow* parent, wxWindowID id,
                             cppw::Sqlite3Connection* connection,
                             int64_t idSeries, wxString title)
: wxDialog(parent, id, "Edit Tags - " + title, wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    m_connection(connection), m_idSeries(idSeries)
{
    m_grid = new wxGrid(this, wxID_ANY);
    m_grid->CreateGrid(0, NUM_COLS);
    m_grid->SetColLabelValue(TAG_COL, "Tag");
    m_grid->SetColLabelValue(VAL_COL, "Value");
    m_grid->HideCol(IDTAG_COL);
    auto stmt = m_connection->PrepareStatement(
        "select idTag, tag, val from Tag where idSeries = ? order by tag, val");
    stmt->Bind(1, m_idSeries);
    auto result = stmt->GetResults();
    while (result->NextRow()) {
        m_grid->AppendRows();
        for (int i = 0; i < result->GetColumnCount(); ++i) {
            m_grid->SetCellValue(
                m_grid->GetNumberRows()-1, i,
                wxString::FromUTF8(result->GetString(i).c_str()));
        }
    }
    AppendWriteProtectedRow(m_grid, VAL_COL, NUM_COLS, false);
    m_grid->AutoSize();
    for (int i = FIRST_VISIBLE_COL; i < NUM_COLS; ++i) {
        if (m_grid->GetColumnWidth(i) < 100) {
            m_grid->SetColumnWidth(i, 100);
        }
    }
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(this, ADD_ROW_BUTTON, "Add Row"),
                   wxSizerFlags(1).Border(0).Expand());
    buttonSizer->Add(new wxButton(this, DELETE_ROW_BUTTON, "Delete Row"),
                   wxSizerFlags(1).Border(0).Expand());
    mainSizer->Add(buttonSizer, wxSizerFlags(0).Border(wxALL).Expand());
    mainSizer->Add(m_grid, wxSizerFlags(1).Border(wxALL).Expand());
    SetSizer(mainSizer);
    SetSizeHints(wxSize(360, 270), wxSize(800, 600));
    Fit();
}

void EditTagDialog::OnAddRowButton(wxCommandEvent& WXUNUSED(event))
{
    m_grid->GoToCell(m_grid->GetNumberRows()-1, TAG_COL);
    m_grid->EnableCellEditControl(true);
}

void EditTagDialog::OnDeleteRowButton(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker lock(m_grid);
    auto rows = m_grid->GetSelectedRows();
    std::sort(&rows[0], &rows[rows.GetCount()-1]);
    if (rows.Last() == m_grid->GetNumberRows()-1) {
        rows.RemoveAt(rows.GetCount()-1);
    }
    if (rows.IsEmpty()) {
        return;
    }
    for(int i = rows.GetCount()-1; i >= 0; --i) {
        HandleDelete(rows[i]);
    }
    m_madeChanges = true;
}

void EditTagDialog::OnGridCellChanging(wxGridEvent& event)
{
    try {
        if (!event.GetString().IsEmpty()) {
            if (event.GetRow() == m_grid->GetNumberRows()-1) {
                HandleInsert(event.GetString());
            } else {
                HandleUpdate(event.GetRow(), event.GetCol(), event.GetString());
            }
            event.Skip();
        } else {
            event.Veto();
            wxMessageBox("Error: Empty tags are not allowed");
            return;
        }
    } catch (cppw::Sqlite3Exception& exception) {
        if (exception.GetErrorCode() == SQLITE_CONSTRAINT) {
            wxMessageBox("Error: Duplicate Tag/Value pair for this show");
            event.Veto();
            return;
        }
        throw;
    }
    m_madeChanges = true;
}

void EditTagDialog::OnGridCellSelect(wxGridEvent& event)
{
    if(event.GetCol() < FIRST_VISIBLE_COL) {
        m_grid->GoToCell(event.GetRow(), FIRST_VISIBLE_COL);
        event.Veto();
    }
    event.Skip();
}

void EditTagDialog::HandleInsert(wxString tag)
{
    auto stmt = m_connection->PrepareStatement(
        "insert into Tag (idSeries, tag) values (?, ?)");
    stmt->Bind(1, m_idSeries);
    stmt->Bind(2, tag.utf8_str());
    auto result = stmt->GetResults();
    result->NextRow();
    m_grid->SetCellValue(m_grid->GetNumberRows()-1, IDTAG_COL,
                         std::to_string(m_connection->GetLastInsertRowID()));
    AppendWriteProtectedRow(m_grid, VAL_COL, NUM_COLS, true);
}

void EditTagDialog::HandleUpdate(int row, int col, wxString val)
{
    std::string colName = col == TAG_COL ? "tag" : "val";
    auto stmt = m_connection->PrepareStatement(
        "update Tag set " + colName + " = ? where idTag = ?");
    stmt->Bind(1, val.utf8_str());
    stmt->Bind(2, m_grid->GetCellValue(row, IDTAG_COL).utf8_str());
    auto result = stmt->GetResults();
    result->NextRow();
}

void EditTagDialog::HandleDelete(int row)
{
    auto stmt = m_connection->PrepareStatement(
        "delete from Tag where idTag = ?");
    stmt->Bind(1, m_grid->GetCellValue(row, IDTAG_COL).utf8_str());
    auto result = stmt->GetResults();
    result->NextRow();
    m_grid->DeleteRows(row);
}
