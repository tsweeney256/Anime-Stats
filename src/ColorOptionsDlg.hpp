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

#ifndef COLOROPTIONSDLG_HPP
#define COLOROPTIONSDLG_HPP

#include <vector>
#include <memory>
#include <wx/dialog.h>
#include "Settings.hpp"

class DataPanel;
class wxBoxSizer;
class wxGridSizer;
class wxPanel;
class wxListBox;
class wxColourPickerCtrl;
class wxCheckBox;
class wxColourPickerEvent;

class ColorOptionsDlg : private wxDialog
{
public:
    ColorOptionsDlg(Settings* settings, DataPanel* dataPanel, wxWindow *parent, wxWindowID id=wxID_ANY);
    //buttons handle changes to dataPanel itself
    using wxDialog::ShowModal;

private:
    void OnListBox(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
    void OnColorCtrl(wxColourPickerEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
    void OnDefault(wxCommandEvent& event);

    void ConstructNumericalPage(int col);
    void ConstructLimitedValsPage(int col);
    void UpdateLayout();
    void SaveChanges();
    //will not use checkbox if checkBoxId is negative
    wxBoxSizer* ConstructItemSizer(wxWindow* parent, wxWindowID id, const wxString& label, long color);

    Settings* m_settings;
    Settings m_tempSettings;
    DataPanel* m_dataPanel;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_bottomColorSizer;
    wxBoxSizer* m_mainSizer;
    wxListBox* m_list;
    wxColourPickerCtrl* m_textColorCtrl;
    wxColourPickerCtrl* m_backgroundColorCtrl;
    wxCheckBox* m_textCheckBox;
    wxCheckBox* m_backgroundCheckBox;
    wxCheckBox* m_valCheckBox;

    std::vector<wxColourPickerCtrl*> m_valColorPickers;

    const char* checkBoxText = "Use system default";

    enum{idOffset = wxID_HIGHEST + 1};

    wxDECLARE_EVENT_TABLE();
};

#endif
