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

#include <utility>
#include <vector>
#include <wx/combobox.h>
#include <wx/msgdlg.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include "MainFrame.hpp"
#include "GroupStatsDlg.hpp"

enum {
    id_group_combo = wxID_HIGHEST + 1,
    id_having_aggregate_combo
};

wxBEGIN_EVENT_TABLE(GroupStatsDlg, wxDialog)
    EVT_COMBOBOX(id_group_combo, GroupStatsDlg::OnGroupCombo)
    EVT_COMBOBOX(id_having_aggregate_combo, GroupStatsDlg::OnAggregateCombo)
wxEND_EVENT_TABLE()

GroupStatsDlg::GroupStatsDlg(wxWindow* parent, MainFrame* top, wxWindowID id,
                             cppw::Sqlite3Connection* connection)
  : wxDialog(parent, id, "Group Stats"), m_connection(connection)
{
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto groupSizer = new wxBoxSizer(wxHORIZONTAL);
    auto groupText = new wxStaticText(this, wxID_ANY, "Group by:");
    auto textSizerFlags = wxSizerFlags(0).Center();
    groupSizer->Add(groupText, textSizerFlags);
    wxArrayString groupComboOptions;
    try {
        auto stmt = m_connection->PrepareStatement(
            "select * from tempSeries where 0");
        auto result = stmt->GetResults();
        groupComboOptions.Add("");
        for (int i = col::PRONUNCIATION + 1; i < col::DATE_STARTED; ++i) {
            groupComboOptions.Add(result->GetColumnName(i));
        }
    } catch (const cppw::Sqlite3Exception& e) {
        wxMessageBox(e.what());
        top->Close(true);
    }
    m_groupCombo = new wxComboBox(
        this, id_group_combo, "", wxDefaultPosition, wxDefaultSize,
        groupComboOptions, wxCB_READONLY);
    auto comboSizerFlags = wxSizerFlags(0).Border(wxLEFT);
    groupSizer->Add(m_groupCombo, comboSizerFlags);
    auto mainSizerFlags = wxSizerFlags(0).Border(wxLEFT | wxBOTTOM | wxRIGHT);
    mainSizer->Add(groupSizer, wxSizerFlags(0).Border(wxALL));

    auto havingSizer = new wxBoxSizer(wxHORIZONTAL);
    auto havingText = new wxStaticText(this, wxID_ANY, "Having:");
    havingSizer->Add(havingText, textSizerFlags);
    wxArrayString havingAggregateOptions;
    havingAggregateOptions.Add("");
    havingAggregateOptions.Add("count");
    havingAggregateOptions.Add("sum");
    havingAggregateOptions.Add("avg");
    havingAggregateOptions.Add("median");
    havingAggregateOptions.Add("stdev");
    m_havingAggregateCombo = new wxComboBox(
        this, id_having_aggregate_combo, "", wxDefaultPosition, wxDefaultSize,
        havingAggregateOptions, wxCB_READONLY);
    havingSizer->Add(m_havingAggregateCombo, comboSizerFlags);
    m_havingColumnCombo = new wxComboBox(
        this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
        groupComboOptions, wxCB_READONLY);
    havingSizer->Add(m_havingColumnCombo, comboSizerFlags);
    wxArrayString havingOperatorOptions;
    havingOperatorOptions.Add("=");
    havingOperatorOptions.Add("!=");
    havingOperatorOptions.Add("<");
    havingOperatorOptions.Add("<=");
    havingOperatorOptions.Add(">");
    havingOperatorOptions.Add(">=");
    m_havingOperatorCombo = new wxComboBox(
        this, wxID_ANY, "=", wxDefaultPosition, wxDefaultSize,
        havingOperatorOptions, wxCB_READONLY);
    havingSizer->Add(m_havingOperatorCombo, comboSizerFlags);
    wxIntegerValidator<int> intValidator;
    intValidator.SetMin(0);
    intValidator.SetMax(999999);
    m_havingValue = new wxTextCtrl(
        this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, intValidator);
    havingSizer->Add(m_havingValue, comboSizerFlags);
    mainSizer->Add(havingSizer, mainSizerFlags);

    auto orderSizer = new wxBoxSizer(wxHORIZONTAL);
    auto orderText = new wxStaticText(this, wxID_ANY, "Order by:");
    orderSizer->Add(orderText, textSizerFlags);
    m_orderOptionsData = {
        {"Group Selection", "Group Selection"},
        {"Anime Time", "Days"},
        {"Anime Time (w/ Rewatches)", "Days with Rewatched"},
        {"Average Rating", "Average"},
        {"Median Rating", "Median"},
        {"Rating Std. Dev", "Std. Dev"},
        {"Rating Count", "Count"},
        {"Watched Count", "Watched"},
        {"Watching Count", "Watching"},
        {"Stalled Count", "Stalled"},
        {"Dropped Count", "Dropped"},
        {"To Watch Count", "To Watch"},
        {"TV Count", "TV"},
        {"OVA Count", "OVA"},
        {"ONA Count", "ONA"},
        {"Movie Count", "Movie"},
        {"TV Special Count", "TV Special"},
    };
    wxArrayString orderOptions;
    for (auto datum : m_orderOptionsData) {
        orderOptions.Add(datum.first);
    }
    m_orderCombo = new wxComboBox(
        this, wxID_ANY, "Group Selection", wxDefaultPosition,
        wxDefaultSize, orderOptions, wxCB_READONLY);
    orderSizer->Add(m_orderCombo, comboSizerFlags);
    m_orderRadioAsc = new wxRadioButton(
        this, wxID_ANY, "asc", wxDefaultPosition,
        wxDefaultSize, wxRB_GROUP);
    orderSizer->Add(m_orderRadioAsc, comboSizerFlags.Center());
    m_orderRadioDesc = new wxRadioButton(
        this, wxID_ANY, "desc");
    orderSizer->Add(m_orderRadioDesc, comboSizerFlags.Center());
    mainSizer->Add(orderSizer, mainSizerFlags);

    auto buttonSizer = CreateButtonSizer(wxOK|wxCANCEL);
    mainSizer->Add(buttonSizer, mainSizerFlags.Right());
    SetSizerAndFit(mainSizer);

    EnableGroupControls(false);
}

void GroupStatsDlg::OnGroupCombo(wxCommandEvent& WXUNUSED(event))
{
    bool enable = !m_groupCombo->GetValue().IsEmpty();
    EnableGroupControls(enable);
}

void GroupStatsDlg::OnAggregateCombo(wxCommandEvent& WXUNUSED(event))
{
    bool enable = !m_havingAggregateCombo->GetValue().IsEmpty();
    m_havingColumnCombo->Enable(enable);
    m_havingOperatorCombo->Enable(enable);
    m_havingValue->Enable(enable);
}

void GroupStatsDlg::EnableGroupControls(bool enable)
{
    m_havingAggregateCombo->Enable(enable);
    if (!enable || !m_havingAggregateCombo->GetValue().IsEmpty()) {
        EnableHavingControls(enable);
    }
    m_orderCombo->Enable(enable);
    m_orderRadioAsc->Enable(enable);
    m_orderRadioDesc->Enable(enable);
}

void GroupStatsDlg::EnableHavingControls(bool enable)
{
    m_havingColumnCombo->Enable(enable);
    m_havingOperatorCombo->Enable(enable);
    m_havingValue->Enable(enable);
}

GroupStatsInfo GroupStatsDlg::GetGroupStatsInfo()
{
    GroupStatsInfo ret;
    ret.groupCol = std::string(m_groupCombo->GetValue().utf8_str());
    if (ret.groupCol.empty()) {
        ret.groupCol = "constant";
    }
    ret.fromTable = "tempSeries";
    if (m_havingAggregateCombo->IsEnabled() &&
        !m_havingAggregateCombo->GetValue().IsEmpty()) {
        ret.havingExpr =
            "having " +
            std::string(m_havingAggregateCombo->GetValue().utf8_str()) +
            "(`" +
            std::string(m_havingColumnCombo->GetValue().utf8_str()) +
            "`) " +
            std::string(m_havingOperatorCombo->GetValue().utf8_str()) +
            " " +
            std::string(m_havingValue->GetValue().utf8_str());
    }
    auto orderEnabled = m_orderCombo->IsEnabled();
    if (orderEnabled) {
        ret.orderCol = std::string(m_orderCombo->GetValue().utf8_str());
    }
    if (!orderEnabled || ret.orderCol == "Group Selection") {
        ret.orderCol = ret.groupCol;
    } else {
        ret.orderCol =
            m_orderOptionsData[m_orderCombo->GetCurrentSelection()].second;
    }
    ret.orderDirection =
        std::string(!m_orderRadioAsc->IsEnabled() ||
                    m_orderRadioAsc->GetValue() ? "asc" : "desc");


    return ret;
}
