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

#include <algorithm>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <fmt/format.h>
#include "cppw/Sqlite3.hpp"
#include "sql/BasicSelect.hpp"
#include "AppIDs.hpp"
#include "Helpers.hpp"
#include "MainFrame.hpp"
#include "QuickFilter.hpp"
#include "AdvSortFrame.hpp"

using namespace fmt::literals;

BEGIN_EVENT_TABLE(AdvSortFrame, wxFrame)
    EVT_CLOSE(AdvSortFrame::OnClose)
    EVT_BUTTON(ID_LEFT_BUTTON, AdvSortFrame::OnLeft)
    EVT_BUTTON(ID_RIGHT_BUTTON, AdvSortFrame::OnRight)
    EVT_BUTTON(ID_UP_BUTTON, AdvSortFrame::OnUp)
    EVT_BUTTON(ID_DOWN_BUTTON, AdvSortFrame::OnDown)
    EVT_BUTTON(wxID_CANCEL, AdvSortFrame::OnCancel)
    EVT_BUTTON(wxID_APPLY, AdvSortFrame::OnApply)
    EVT_BUTTON(wxID_OK, AdvSortFrame::OnOk)
    EVT_RADIOBUTTON(ID_RADIO_LEFT, AdvSortFrame::OnRadioLeft)
    EVT_LISTBOX(ID_LISTBOX_LEFT, AdvSortFrame::OnListBoxLeftSelect)
END_EVENT_TABLE()

AdvSortFrame::AdvSortFrame(QuickFilter* quickFilter, MainFrame* top,
                           wxWindowID id, cppw::Sqlite3Connection* connection)
    : wxFrame(quickFilter, id, "Advanced Sorting",
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER |wxMAXIMIZE_BOX)),
      m_top(top), m_quickFilter(quickFilter)
{
    auto mainPanel = new wxPanel(this, wxID_ANY);

    m_sortList = new wxListBox(mainPanel, ID_LISTBOX_LEFT);
    auto upBtn = new wxButton(mainPanel, ID_UP_BUTTON, _("Up"));
    auto downBtn = new wxButton(mainPanel, ID_DOWN_BUTTON, _("Down"));
    m_ascBtnLeft = new wxRadioButton(mainPanel, ID_RADIO_LEFT, _("Ascending"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_descBtnLeft = new wxRadioButton(mainPanel, ID_RADIO_LEFT, _("Descending"));
    auto leftBtn = new wxButton(mainPanel, ID_LEFT_BUTTON, "<--");
    auto rightBtn = new wxButton(mainPanel, ID_RIGHT_BUTTON, "-->");
    m_dontList = new wxListBox(mainPanel, wxID_ANY);
    m_ascBtnRight = new wxRadioButton(mainPanel, wxID_ANY, _("Ascending"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_descBtnRight = new wxRadioButton(mainPanel, wxID_ANY, _("Descending"));
    auto applyBtn = new wxButton(mainPanel, wxID_APPLY);
    auto okBtn = new wxButton(mainPanel, wxID_OK);
    auto cancelBtn = new wxButton(mainPanel, wxID_CANCEL);
    auto sortOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Sort By"));
    auto dontOutlineSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, _("Don't Sort By"));

    auto upDownSizer = new wxBoxSizer(wxHORIZONTAL);
    auto radioSizerLeft = new wxBoxSizer(wxHORIZONTAL);
    auto radioSizerRight = new wxBoxSizer(wxHORIZONTAL);
    auto midSizer = new wxBoxSizer(wxVERTICAL);
    auto contentSizer = new wxBoxSizer(wxHORIZONTAL);
    auto bottomBtnSizer = new wxStdDialogButtonSizer();
    auto fullSizer = new wxBoxSizer(wxVERTICAL);

    auto upDownRadioFlags = wxSizerFlags(0).Border(wxALL).Center().Bottom();
    auto midFlags = wxSizerFlags(wxSizerFlags(0).Border(wxALL).Center());
    auto borderNoExpandFlag = wxSizerFlags(0).Border(wxALL);
    auto expandFlag = wxSizerFlags(1).Border(wxALL).Expand();

    upDownSizer->Add(upBtn, upDownRadioFlags);
    upDownSizer->Add(downBtn, upDownRadioFlags);
    radioSizerLeft->Add(m_ascBtnLeft, upDownRadioFlags);
    radioSizerLeft->Add(m_descBtnLeft, upDownRadioFlags);
    sortOutlineSizer->Add(m_sortList, expandFlag);
    sortOutlineSizer->Add(upDownSizer, upDownRadioFlags);
    sortOutlineSizer->Add(radioSizerLeft, upDownRadioFlags);

    midSizer->Add(leftBtn, midFlags);
    midSizer->Add(rightBtn, midFlags);

    radioSizerRight->Add(m_ascBtnRight, upDownRadioFlags);
    radioSizerRight->Add(m_descBtnRight, upDownRadioFlags);
    dontOutlineSizer->Add(m_dontList, expandFlag);
    dontOutlineSizer->Add(radioSizerRight, borderNoExpandFlag);

    contentSizer->Add(sortOutlineSizer, expandFlag);
    contentSizer->Add(midSizer, midFlags);
    contentSizer->Add(dontOutlineSizer, borderNoExpandFlag);

    bottomBtnSizer->AddButton(applyBtn);
    bottomBtnSizer->AddButton(okBtn);
    bottomBtnSizer->AddButton(cancelBtn);
    bottomBtnSizer->Realize();

    fullSizer->Add(contentSizer, borderNoExpandFlag);
    fullSizer->Add(bottomBtnSizer, wxSizerFlags(0).Border(wxALL).Bottom().Right());

    wxArrayString cols;
    std::string stmtStr = SqlStrings::basicSelect + " where 1<>1";
    stmtStr = fmt::format(stmtStr, "tag_cols"_a="");
    auto stmt = connection->PrepareStatement(stmtStr);
    auto result = stmt->GetResults();
    result->NextRow();
    for (int i = col::FIRST_VISIBLE_COL; i < col::NUM_COLS; ++i) {
        cols.Add(result->GetColumnName(i));
    }
    m_dontList->InsertItems(cols, 0);
    mainPanel->SetSizerAndFit(fullSizer);
    this->Fit();
}

void AdvSortFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void AdvSortFrame::OnLeft(wxCommandEvent& WXUNUSED(event))
{
    if(m_dontList->GetSelection() != wxNOT_FOUND){
        auto selectedName = m_dontList->GetString(m_dontList->GetSelection());
        auto sortName = selectedName == "Title" ? "nameSort" : selectedName;
        m_toSort.emplace_back(sortName, m_ascBtnRight->GetValue());
        auto nameStr = m_dontList->GetString((m_dontList->GetSelection()));
        auto ascStr = wxString((m_ascBtnRight->GetValue() ? "ASC" : "DESC"));
        wxString str =
            (nameStr == "nameSort" ? "Title" : nameStr) + "   " + ascStr;
        m_sortList->InsertItems(1, &str, m_sortList->GetCount());
        m_dontList->Delete(m_dontList->GetSelection());
    }
}

void AdvSortFrame::OnRight(wxCommandEvent& WXUNUSED(event))
{
    if(m_sortList->GetSelection() != wxNOT_FOUND){
        auto toSortName = m_toSort[m_sortList->GetSelection()].name;
        auto displayName = toSortName == "nameSort" ? "Title" : toSortName;
        m_dontList->InsertItems(1, &displayName, m_dontList->GetCount());
        m_toSort.erase(m_toSort.begin() + m_sortList->GetSelection());
        m_sortList->Delete(m_sortList->GetSelection());
    }
}

void AdvSortFrame::OnUp(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(1);
}

void AdvSortFrame::OnDown(wxCommandEvent& WXUNUSED(event))
{
    UpDownCommon(-1);
}

void AdvSortFrame::OnRadioLeft(wxCommandEvent& WXUNUSED(event))
{
    if(m_sortList->GetSelection() != wxNOT_FOUND){
        auto nameStr = m_toSort[m_sortList->GetSelection()].name;
        auto ascStr = (m_ascBtnLeft->GetValue() ? _("ASC") : _("DESC"));
        wxString str =
            (nameStr == "nameSort" ? "Title" : nameStr) + "   " + ascStr;
        m_toSort[m_sortList->GetSelection()].asc = m_ascBtnLeft->GetValue();
        m_sortList->SetString(m_sortList->GetSelection(), str);
    }
}

void AdvSortFrame::OnListBoxLeftSelect(wxCommandEvent& event)
{
    (m_toSort[event.GetInt()].asc ? m_ascBtnLeft : m_descBtnLeft)->SetValue(true);
}

void AdvSortFrame::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void AdvSortFrame::OnApply(wxCommandEvent& WXUNUSED(event))
{
    ApplyOkCommon();
}

void AdvSortFrame::OnOk(wxCommandEvent& WXUNUSED(event))
{
    ApplyOkCommon();
    Close();
}

void AdvSortFrame::UpDownCommon(int direction)
{
    auto idx = m_sortList->GetSelection();

    if(idx == wxNOT_FOUND)
        return;
    else if(idx - direction < 0)
        return;
    else if(idx - direction + 1 > static_cast<int>(m_sortList->GetCount()))
        return;

    auto tempStr = m_sortList->GetString(idx);

    m_sortList->Delete(idx);
    m_sortList->InsertItems(1, &tempStr, idx - direction);
    m_sortList->SetSelection(idx - direction);
    std::iter_swap(m_toSort.begin() + idx, m_toSort.begin() + idx - direction);
}

void AdvSortFrame::ApplyOkCommon()
{
    m_quickFilter->SetSort(m_toSort);
    m_top->UpdateStats();
}
