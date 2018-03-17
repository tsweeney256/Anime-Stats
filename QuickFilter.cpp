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

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include "cppw/Sqlite3.hpp"
#include "FilterStructs.hpp"
#include "SortStruct.hpp"
#include "DbFilter.hpp"
#include "MainFrame.hpp"
#include "QuickFilter.hpp"

enum {
    id_select_combo = wxID_HIGHEST + 1,
    id_title_text_field,
    id_new_btn,
    id_overwrite_btn,
    id_delete_btn
};

wxBEGIN_EVENT_TABLE(QuickFilter, wxPanel)
    EVT_TEXT_ENTER(wxID_ANY, QuickFilter::OnProcessEnter)
    EVT_TEXT(id_title_text_field, QuickFilter::OnTitleText)
    EVT_COMBOBOX(id_select_combo, QuickFilter::OnComboBoxChange)
    EVT_BUTTON(id_new_btn, QuickFilter::OnQuickFilterNew)
    EVT_BUTTON(id_overwrite_btn, QuickFilter::OnQuickFilterOverwrite)
    EVT_BUTTON(id_delete_btn, QuickFilter::OnQuickFilterDelete)
wxEND_EVENT_TABLE()

QuickFilter::QuickFilter(wxWindow* parent, MainFrame* top, wxWindowID id,
                         cppw::Sqlite3Connection* connection)
    : wxPanel(parent, id), m_dbFilter(std::make_unique<DbFilter>()), m_top(top)
{
    m_quickFilterSelect = new wxComboBox(
        this, id_select_combo, "", wxDefaultPosition, wxDefaultSize, 0, nullptr,
        wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_quickFilterTitle = new wxTextCtrl(
        this, id_title_text_field, wxEmptyString,
        wxDefaultPosition, wxSize(250, -1), wxTE_PROCESS_ENTER);
    auto quickFilterNewButton = new wxButton(
        this, id_new_btn, "New");
    auto quickFilterOverwriteButton = new wxButton(
        this, id_overwrite_btn, "Overwrite");
    auto quickFilterDeleteButton = new wxButton(
        this, id_delete_btn, "Delete");

    auto quickFilterTypeSizer = new wxStaticBoxSizer(
        wxHORIZONTAL, this, _("Quick Filter Type"));
    auto quickFilterTitleSizer = new wxStaticBoxSizer(
        wxHORIZONTAL, this, _("Quick Filter Title"));
    auto quickFilterSizer = new wxBoxSizer(wxHORIZONTAL);

    quickFilterTypeSizer->Add(
        m_quickFilterSelect, wxSizerFlags(3).Border(wxALL).Center().Expand());
    auto qfButtonFlags = wxSizerFlags(1).Center();
    quickFilterTypeSizer->Add(quickFilterNewButton, qfButtonFlags);
    quickFilterTypeSizer->Add(quickFilterOverwriteButton, qfButtonFlags);
    quickFilterTypeSizer->Add(
        quickFilterDeleteButton, qfButtonFlags.Border(wxRIGHT));

    quickFilterTitleSizer->Add(
        m_quickFilterTitle, wxSizerFlags(1).Border(wxALL).Center());

    quickFilterSizer->Add(quickFilterTypeSizer, wxSizerFlags(2).Border(wxRIGHT));
    quickFilterSizer->Add(quickFilterTitleSizer, wxSizerFlags(1));

    SetSizerAndFit(quickFilterSizer);
    Reset(connection);
}

void QuickFilter::Reset(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
    m_dbFilter->Reset(connection);
    m_quickFilterSelect->Clear();
    for (auto& name : m_dbFilter->GetFilterNames()) {
        m_quickFilterSelect->Append(wxString::FromUTF8(name.c_str()));
    }
    m_quickFilterSelect->SetValue(m_dbFilter->GetDefaultFilterName());
}

ConstFilter QuickFilter::GetFilter()
{
    const auto& cDbFilter = *m_dbFilter;
    return cDbFilter.GetFilter();
}

const std::vector<colSort>* QuickFilter::GetSort()
{
    const auto& cDbFilter = *m_dbFilter;
    return cDbFilter.GetSort();
}

void QuickFilter::LoadDefaultFilter()
{
    m_dbFilter->LoadFilter(m_dbFilter->GetDefaultFilterName());
}

void QuickFilter::SetFilter(std::unique_ptr<BasicFilterInfo> bfi,
                            std::unique_ptr<AdvFilterInfo> afi)
{
    m_dbFilter->SetFilter(std::move(bfi), std::move(afi));
}

void QuickFilter::SetSort(std::vector<colSort> sortingRules)
{
    m_dbFilter->SetSort(sortingRules);
}

void QuickFilter::SetDefaultFilter(wxString name)
{
    m_dbFilter->SetDefaultFilter(std::string(name.utf8_str()));
}

wxString QuickFilter::GetSelectedFilterName()
{
    return m_quickFilterSelect->GetValue();
}

void QuickFilter::OnProcessEnter(wxCommandEvent& event)
{
    auto clone = event.Clone();
    clone->SetEventType(QuickFilterProcessEnterEvent);
    wxQueueEvent(this, clone);
}

void QuickFilter::OnTitleText(wxCommandEvent& event)
{
    auto filter = m_dbFilter->GetFilter();
    filter.first->title = std::string(event.GetString().utf8_str());
}

void QuickFilter::OnComboBoxChange(wxCommandEvent& WXUNUSED(event))
{
    m_dbFilter->LoadFilter(
        std::string(m_quickFilterSelect->GetValue().utf8_str()));
}

void QuickFilter::OnQuickFilterNew(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, "New Quick Filter");
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto topSizer = new wxBoxSizer(wxHORIZONTAL);
    auto buttonSizer = dlg.CreateButtonSizer(wxOK|wxCANCEL);
    auto nameCtrl = new wxTextCtrl(&dlg, wxID_ANY);
    auto defaultBtn = new wxCheckBox(&dlg, wxID_ANY, "Default Filter");
    topSizer->Add(nameCtrl, wxSizerFlags(2).Border(wxALL));
    topSizer->Add(defaultBtn, wxSizerFlags(1).Border(wxALL ^ wxLEFT));
    mainSizer->Add(topSizer);
    mainSizer->Add(buttonSizer, wxSizerFlags(0).Border(wxALL ^ wxTOP).Right());
    dlg.SetSizerAndFit(mainSizer);
    if (dlg.ShowModal() == wxID_OK) {
        if (nameCtrl->GetValue() == "") {
            wxMessageBox("Error: Empty filter names are not allowed");
            return;
        }
        try {
            auto isNewDefaultFilter = defaultBtn->GetValue();
            m_dbFilter->SaveFilter(std::string(nameCtrl->GetValue().utf8_str()),
                                   isNewDefaultFilter);
            //cheeky way of alphabetizing the combobox entries
            m_quickFilterSelect->Clear();
            for (auto& name : m_dbFilter->GetFilterNames()) {
                m_quickFilterSelect->Append(name);
            }
            m_quickFilterSelect->SetValue(nameCtrl->GetValue());
            m_top->SetUnsavedChanges(true);
        } catch (const cppw::Sqlite3Exception& e) {
            if (e.GetErrorCode() == SQLITE_CONSTRAINT) {
                wxMessageBox("Error: Filter with this name already exists");
            } else {
                wxMessageBox(wxString("Error: ") + e.what());
                m_top->Close(true);
            }
        }
    }
}

void QuickFilter::OnQuickFilterOverwrite(wxCommandEvent& WXUNUSED(event))
{
    auto curFilter = std::string(m_quickFilterSelect->GetValue().utf8_str());
    if (curFilter == "") {
        wxMessageBox("Error: No filter selected. Can not overwrite");
        return;
    }
    try {
        wxMessageDialog dlg(
            this, "Are you sure you want to overwrite the \"" +
            wxString::FromUTF8(curFilter.c_str()) +
            "\" filter with the current filter settings?",
            wxMessageBoxCaptionStr, wxOK|wxCANCEL|wxRIGHT);
        if (dlg.ShowModal() == wxID_OK) {
            auto isDefault = m_dbFilter->GetDefaultFilterName() == curFilter;
            m_dbFilter->DeleteFilter(curFilter);
            m_dbFilter->SaveFilter(curFilter, isDefault);
            m_quickFilterSelect->RemoveSelection();
            m_top->SetUnsavedChanges(true);
        }
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(wxString("Error: ") + e.what());
        m_top->Close(true);
    }
}

void QuickFilter::OnQuickFilterDelete(wxCommandEvent& WXUNUSED(event))
{
    auto curFilter = std::string(m_quickFilterSelect->GetValue().utf8_str());
    if (curFilter == "") {
        wxMessageBox("Error: No filter selected. Can not Delete");
        return;
    }
    try {
        wxMessageDialog dlg(
            this, "Are you sure you want to delete the \"" +
            wxString::FromUTF8(curFilter.c_str()) +
            "\" filter?", wxMessageBoxCaptionStr, wxOK|wxCANCEL|wxRIGHT);
        if (dlg.ShowModal() == wxID_OK) {
            m_dbFilter->DeleteFilter(curFilter);
            //reuse old functions instead of risking bugs
            m_quickFilterSelect->Clear();
            for (auto& name : m_dbFilter->GetFilterNames()) {
                m_quickFilterSelect->Append(name);
            }
            //to ensure the readonly comboxbox value is blank
            m_quickFilterSelect->Append("");
            m_quickFilterSelect->SetValue("");
            m_quickFilterSelect->Delete(m_quickFilterSelect->GetSelection());
            m_top->SetUnsavedChanges(true);
        }
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(wxString("Error: ") + e.what());
        m_top->Close(true);
    }
}

wxDEFINE_EVENT(QuickFilterProcessEnterEvent, wxCommandEvent);
