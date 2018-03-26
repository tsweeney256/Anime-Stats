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

#include <wx/datetime.h>
#include <wx/msgdlg.h>
#include "AppIDs.hpp"
#include "DataGrid.hpp"

wxBEGIN_EVENT_TABLE(DataGrid, wxGrid)
    EVT_CHAR(DataGrid::OnChar)
wxEND_EVENT_TABLE()

DataGrid::DataGrid(wxWindow* parent, wxWindowID id)
    : wxGrid(parent, id)
{}

void DataGrid::IncreaseCellInt(int row, int col, int amount)
{
    long val;
    auto str = GetCellValue(row, col);
    str = str == "" ? "0" : str;
    str.ToLong(&val);
    val += amount;
    if (val < 0) {
        val = 0;
    } else if (val > 999999) {
        val = 999999;
    }
    SetCellValue(row, col, wxString::Format("%ld", val));
}

void DataGrid::IncreaseCellDate(int row, int col, int amount)
{
    wxDateTime date;
    auto str = GetCellValue(row, col);
    bool dontAdd = false;
    if (str == "") {
        dontAdd = true;
        str = wxDateTime::Now().FormatISODate();
    }
    date.ParseISODate(str);
    if (!dontAdd) {
        wxDateSpan diff(0, 0, 0, amount);
        date += diff;
        if (date.GetYear() > 9999 || date.GetYear() < 0) {
            return;
        }
    }
    SetCellValue(row, col, date.FormatISODate());
}

void DataGrid::OnChar(wxKeyEvent& event)
{
    auto row = GetGridCursorRow();
    auto col = GetGridCursorCol();
    bool didWork = false;
    if (col::isColNumeric(col)) {
        if (event.AltDown() && event.GetUnicodeKey() == '+') {
            IncreaseCellInt(row, col, 1);
            didWork = true;
        } else if (event.AltDown() && event.GetUnicodeKey() == '-') {
            IncreaseCellInt(row, col, -1);
            didWork = true;
        }
    } else if (col == col::DATE_STARTED || col == col::DATE_FINISHED) {
        if (event.AltDown() && event.GetUnicodeKey() == '+') {
            IncreaseCellDate(row, col, 1);
            didWork = true;
        } else if (event.AltDown() && event.GetUnicodeKey() == '-') {
            IncreaseCellDate(row, col, -1);
            didWork = true;
        }
    }
    if(!didWork) {
        event.Skip();
    }
}