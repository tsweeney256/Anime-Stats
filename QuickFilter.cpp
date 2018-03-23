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

#include <iomanip>
#include <sstream>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <fmt/format.h>
#include "cppw/Sqlite3.hpp"
#include "DbFilter.hpp"
#include "Helpers.hpp"
#include "FilterStructs.hpp"
#include "MainFrame.hpp"
#include "SortStruct.hpp"
#include "QuickFilter.hpp"

using namespace fmt::literals;

namespace
{
enum {
    id_select_combo = wxID_HIGHEST + 1,
    id_title_text_field,
    id_new_btn,
    id_overwrite_btn,
    id_delete_btn
};
}

wxBEGIN_EVENT_TABLE(QuickFilter, wxPanel)
    EVT_TEXT_ENTER(wxID_ANY, QuickFilter::OnProcessEnter)
    EVT_TEXT(id_title_text_field, QuickFilter::OnTitleText)
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
        m_quickFilterSelect, wxSizerFlags(3).Border(wxALL).Expand());
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
    wxString basicSelectStringTemp;
    readFileIntoString(basicSelectStringTemp, "basicSelect.sql", m_top);
    m_basicSelectString = std::string(basicSelectStringTemp.utf8_str());
}

ConstFilter QuickFilter::GetFilter() const
{
    const auto& cDbFilter = *m_dbFilter;
    return cDbFilter.GetFilter();
}

const std::vector<colSort>* QuickFilter::GetSort() const
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

void QuickFilter::SetFilter(std::string name)
{
    m_dbFilter->LoadFilter(name);
    m_dbFilter->GetFilter().first->title =
        std::string(m_quickFilterTitle->GetValue().utf8_str());
}

void QuickFilter::SetSort(std::vector<colSort> sortingRules)
{
    m_dbFilter->SetSort(sortingRules);
}

void QuickFilter::SetDefaultFilter(std::string name)
{
    m_dbFilter->SetDefaultFilter(name);
}

wxString QuickFilter::GetSelectedFilterName() const
{
    return m_quickFilterSelect->GetValue();
}

cppw::Sqlite3Result* QuickFilter::GetAnimeData(
    bool filtered,  bool sorted, bool useTempTable, bool useTagCols)
{
    std::string tagColsStr =
        useTagCols ?
        ", tag as Tag, val as `Tag Value`" :
        ", 1 as Tag, 1 as `Tag Value`";
    auto sqlStr = fmt::format(m_basicSelectString, "tag_cols"_a=tagColsStr);
    const auto* cThis = this;
    ConstFilter filter = cThis->GetFilter();
    auto newBasicFilterInfo = filter.first;
    auto newAdvFilterInfo = filter.second;
    bool usingTagKey = false;
    bool usingTagVal = false;
    if (filtered) {
        std::string watchedStatus = "idWatchedStatus";
        std::string releaseType = "idReleaseType";
        std::string season = "idSeason";
        bool firstStatus = true;
        //if none of the boxes are ticked then nothing should be displayed
        bool showNothing = false;
        std::stringstream statusStr;

        //watchedStatus
        if(newBasicFilterInfo->watched)
            AppendStatusStr(statusStr, watchedStatus + "= 1 ", firstStatus);
        if(newBasicFilterInfo->watching)
            AppendStatusStr(statusStr, watchedStatus + "= 2 ", firstStatus);
        if(newBasicFilterInfo->stalled)
            AppendStatusStr(statusStr, watchedStatus + "= 3 ", firstStatus);
        if(newBasicFilterInfo->dropped)
            AppendStatusStr(statusStr, watchedStatus + "= 4 ", firstStatus);
        if(newBasicFilterInfo->toWatch)
            AppendStatusStr(statusStr, watchedStatus + "= 5 ", firstStatus);
        if(newBasicFilterInfo->watchedBlank)
            AppendStatusStr(statusStr, watchedStatus + "= 0 ", firstStatus);
        if(!firstStatus)
            statusStr << " ) ";
        else
            showNothing = true;

        if(newAdvFilterInfo){
            firstStatus = true;

            firstStatus = true;
            if(newAdvFilterInfo->tv)
                AppendStatusStr(statusStr, releaseType + "= 1 ", firstStatus);
            if(newAdvFilterInfo->ova)
                AppendStatusStr(statusStr, releaseType + "= 2 ", firstStatus);
            if(newAdvFilterInfo->ona)
                AppendStatusStr(statusStr, releaseType + "= 3 ", firstStatus);
            if(newAdvFilterInfo->movie)
                AppendStatusStr(statusStr, releaseType + "= 4 ", firstStatus);
            if(newAdvFilterInfo->tvSpecial)
                AppendStatusStr(statusStr, releaseType + "= 5 ", firstStatus);
            if(newAdvFilterInfo->releaseBlank)
                AppendStatusStr(statusStr, releaseType + "= 0 ", firstStatus);
            if(!firstStatus)
                statusStr << " ) ";
            else
                showNothing = true;

            firstStatus = true;
            if(newAdvFilterInfo->winter)
                AppendStatusStr(statusStr, season + "= 1 ", firstStatus);
            if(newAdvFilterInfo->spring)
                AppendStatusStr(statusStr, season + "= 2 ", firstStatus);
            if(newAdvFilterInfo->summer)
                AppendStatusStr(statusStr, season + "= 3 ", firstStatus);
            if(newAdvFilterInfo->fall)
                AppendStatusStr(statusStr, season + "= 4 ", firstStatus);
            if(newAdvFilterInfo->seasonBlank)
                AppendStatusStr(statusStr, season + "= 0 ", firstStatus);
            if(!firstStatus)
                statusStr << " ) ";
            else
                showNothing = true;

            if(newAdvFilterInfo->ratingEnabled)
                statusStr << " and (rating between " <<
                    newAdvFilterInfo->ratingLow << " and " <<
                    newAdvFilterInfo->ratingHigh << ") ";
            if(newAdvFilterInfo->yearEnabled)
                statusStr << " and (year between " <<
                    newAdvFilterInfo->yearLow << " and "
                          << newAdvFilterInfo->yearHigh << ") ";
            if(newAdvFilterInfo->epsWatchedEnabled)
                statusStr << " and (episodesWatched between " <<
                    newAdvFilterInfo->epsWatchedLow <<
                    " and " << newAdvFilterInfo->epsWatchedHigh <<") ";
            if(newAdvFilterInfo->totalEpsEnabled)
                statusStr << " and (totalEpisodes between " <<
                    newAdvFilterInfo->totalEpsLow <<
                    " and " << newAdvFilterInfo->totalEpsHigh << ") ";
            if(newAdvFilterInfo->epsRewatchedEnabled)
                statusStr << " and (rewatchedEpisodes between " <<
                    newAdvFilterInfo->epsRewatchedLow <<
                    " and " << newAdvFilterInfo->epsRewatchedHigh << ") ";
            if(newAdvFilterInfo->lengthEnabled)
                statusStr << " and (episodeLength between " <<
                    newAdvFilterInfo->lengthLow << " and " <<
                    newAdvFilterInfo->lengthHigh << ") ";
            if(newAdvFilterInfo->dateStartedEnabled){
                statusStr << " and (dateStarted between date('";
                statusStr << std::setfill('0') << std::setw(4) <<
                    newAdvFilterInfo->yearStartedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->monthStartedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->dayStartedLow;
                statusStr << "') and date('";
                statusStr << std::setfill('0') << std::setw(4) <<
                    newAdvFilterInfo->yearStartedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->monthStartedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->dayStartedHigh;
                statusStr << "')) ";
            }
            if(newAdvFilterInfo->dateFinishedEnabled){
                statusStr << " and (dateFinished between date('";
                statusStr << std::setfill('0') << std::setw(4) <<
                    newAdvFilterInfo->yearFinishedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->monthFinishedLow;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->dayFinishedLow;
                statusStr << "') and date('";
                statusStr << std::setfill('0') << std::setw(4) <<
                    newAdvFilterInfo->yearFinishedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->monthFinishedHigh;
                statusStr << "-";
                statusStr << std::setfill('0') << std::setw(2) <<
                    newAdvFilterInfo->dayFinishedHigh;
                statusStr << "')) ";
            }
            if(newAdvFilterInfo->tagKeyEnabled &&
               !newAdvFilterInfo->tagKeyInverse) {
                usingTagKey = true;
                statusStr << " and tag like ?2 ";
            }
            if(newAdvFilterInfo->tagValEnabled &&
               !newAdvFilterInfo->tagValInverse) {
                usingTagVal = true;
                statusStr << " and val like ?3 ";
            }
        }
        if (useTempTable) {
            try {
                auto deleteStmt = m_connection->PrepareStatement(
                    "delete from tempSeries");
                auto deleteResult = deleteStmt->GetResults();
                deleteResult->NextRow();
                sqlStr = "insert into tempSeries " + sqlStr;
            } catch (const cppw::Sqlite3Exception& e) {
                wxMessageBox(e.what());
                m_top->Close(true);
            }
        }
        //1=1 is just a dumb hack so I don't have to worry about when
        //to start using 'and's and 'or's
        sqlStr += " where 1=1 " +
            (showNothing ? " and 1 <> 1 " : statusStr.str());
        if (newAdvFilterInfo->tagKeyInverse || newAdvFilterInfo->tagValInverse) {
            sqlStr += "\nexcept\n" + sqlStr;
        }
        if(newAdvFilterInfo->tagKeyEnabled &&
           newAdvFilterInfo->tagKeyInverse) {
            usingTagKey = true;
            sqlStr += " and tag like ?2 ";
        }
        if(newAdvFilterInfo->tagValEnabled &&
           newAdvFilterInfo->tagValInverse) {
            usingTagVal = true;
            sqlStr += " and val like ?3 ";
        }
    }
    if (sorted) {
        sqlStr += " order by " + CreateSortStr();
    }
    try {
        m_animeStatsStmt = m_connection->PrepareStatement(sqlStr);
        if (filtered) {
            std::string bindStr = "%" + newBasicFilterInfo->title + "%";
            m_animeStatsStmt->Bind(1, bindStr);
            if (usingTagKey) {
                m_animeStatsStmt->Bind(2, newAdvFilterInfo->tagKey);
            }
            if (usingTagVal) {
                m_animeStatsStmt->Bind(3, newAdvFilterInfo->tagVal);
            }
        }
        m_animeStatsResults = m_animeStatsStmt->GetResults();
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(e.what());
        m_top->Close(true);
    }
    return m_animeStatsResults.get();
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

void QuickFilter::AppendStatusStr(
    std::stringstream& statusStr, std::string toAppend, bool& firstStatus)
{
    if(!firstStatus)
        statusStr << " or ";
    else{
        statusStr << " and ( ";
        firstStatus = false;
    }
    statusStr << toAppend;
}

std::string QuickFilter::CreateSortStr()
{
    const auto* cThis = this;
    const auto& sortingRules = *cThis->GetSort();
    std::string sortStr;
    for(size_t i = 0; i < sortingRules.size(); ++i) {
        sortStr += "`" + sortingRules[i].name + "` collate nocase " +
            (sortingRules[i].asc ? "asc " : "desc ") +
            (i + 1 == sortingRules.size() ? "" : ", ");
    }
    return sortStr;
}

wxDEFINE_EVENT(QuickFilterProcessEnterEvent, wxCommandEvent);
