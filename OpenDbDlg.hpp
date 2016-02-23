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

#ifndef OPENDBDLG_HPP
#define OPENDBDLG_HPP

#include <wx/dialog.h>
#include <wx/event.h>

class OpenDbDlg : private wxDialog
{
public:
    OpenDbDlg(wxString& file, wxString msg, wxWindow* parent, wxWindowID id);
    using wxDialog::ShowModal;

private:
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    wxString& m_file;

    DECLARE_EVENT_TABLE()
};

#endif
