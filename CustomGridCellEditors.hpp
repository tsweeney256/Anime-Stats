/*Anime Stats
Copyright (C) 2016 Thomas Sweeney
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

#ifndef CUSTOMGRIDCELLEDITORS_HPP
#define CUSTOMGRIDCELLEDITORS_HPP

#include <wx/grid.h>
#include <wx/valtext.h>

//enforces yyyy-mm-dd format
class GridCellDateEditor : public wxGridCellTextEditor
{
public:
    GridCellDateEditor();
    virtual void BeginEdit(int row, int col, wxGrid *grid) override;
    virtual bool EndEdit(int row, int col, const wxGrid *grid, const wxString &oldval, wxString *newval) override;
    virtual void Create (wxWindow *parent, wxWindowID id, wxEvtHandler *evtHandler) override;
    //this is only called upon the wxEVT_CHAR event when the cell is selected, not when it is in editing mode
    virtual bool IsAcceptedKey(wxKeyEvent& event) override;

private:
    bool m_showHint = true;
};

class DateValidator : public wxTextValidator
{
public:
    DateValidator();
    DateValidator(const DateValidator&) = default;
    virtual wxObject* Clone() const override { return new DateValidator(*this); }

private:
    void OnKeyDown(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
