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
#include <wx/grid.h>

class MainFrame;

class DataGrid : public wxGrid
{
public:
    DataGrid(wxWindow* parent, MainFrame* top, wxWindowID id);
    void IncreaseCellInt(int row, int col, int amount);
    void IncreaseCellDate(int row, int col, int amount);
    void SetCellValueWithEvents(int row, int col, const wxString& val);

private:
    void OnChar(wxKeyEvent& key);
    void OnKeyDown(wxKeyEvent& key);

    MainFrame* m_top;

    wxDECLARE_EVENT_TABLE();
};
