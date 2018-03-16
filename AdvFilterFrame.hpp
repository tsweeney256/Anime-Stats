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

#ifndef ADVFILTERFRAME_HPP
#define ADVFILTERFRAME_HPP

#include <wx/frame.h>
#include <wx/scrolwin.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

class MainFrame;
class QuickFilter;

class AdvFilterFrame : public wxFrame
{
public:
    AdvFilterFrame(QuickFilter* parent, MainFrame* top, const wxString& title,
                   const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);
    void OnWatchedStatusCheckAllBtn(wxCommandEvent& event);
    void OnWatchedStatusUnCheckAllBtn(wxCommandEvent& event);
    void OnReleaseTypeCheckAllBtn(wxCommandEvent& event);
    void OnReleaseTypeUnCheckAllBtn(wxCommandEvent& event);
    void OnSeasonCheckAllBtn(wxCommandEvent& event);
    void OnSeasonUnCheckAllBtn(wxCommandEvent& event);
    void OnRatingEnableCheckBox(wxCommandEvent& event);
    void OnYearEnableCheckBox(wxCommandEvent& event);
    void OnEpsWatchedEnableCheckBox(wxCommandEvent& event);
    void OnEpsRewatchedEnableCheckBox(wxCommandEvent& event);
    void OnTotalEpsEnableCheckBox(wxCommandEvent& event);
    void OnLengthEnableCheckBox(wxCommandEvent& event);
    void OnDateStartedEnableCheckBox(wxCommandEvent& event);
    void OnDateFinishedEnableCheckBox(wxCommandEvent& event);
    void OnTagKeyEnableCheckBox(wxCommandEvent& event);
    void OnTagValEnableCheckBox(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);

    wxStaticText* getLabel(const wxString& text);
    void EnableAllWatchedStatus(bool val = true);
    void EnableAllReleaseType(bool val = true);
    void EnableAllSeason(bool val = true);
    void ApplyFilter();

    const int m_minNum    = 0;
    const int m_maxNum    = 999999;
    const int m_minYear   = 0;
    const int m_maxYear   = 9999;
    const int m_minMonth  = 1;
    const int m_maxMonth  = 12;
    const int m_minDay    = 1;
    const int m_maxDay    = 31;

    MainFrame* m_top;
    QuickFilter* m_quickFilter;
    wxScrolledWindow* m_mainPanel;

    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_watchedStatusBlankCheck;
    wxCheckBox* m_toWatchCheck;
    wxCheckBox* m_tvCheck;
    wxCheckBox* m_ovaCheck;
    wxCheckBox* m_onaCheck;
    wxCheckBox* m_movieCheck;
    wxCheckBox* m_tvSpecialCheck;
    wxCheckBox* m_releaseTypeBlankCheck;
    wxCheckBox* m_winterCheck;
    wxCheckBox* m_springCheck;
    wxCheckBox* m_summerCheck;
    wxCheckBox* m_fallCheck;
    wxCheckBox* m_seasonBlankCheck;

    wxTextCtrl* m_titleTextField;
    wxTextCtrl* m_tagKeyTextField;
    wxTextCtrl* m_tagValTextField;

    wxCheckBox* m_ratingsEnabled;
    wxCheckBox* m_yearEnabled;
    wxCheckBox* m_epsWatchedEnabled;
    wxCheckBox* m_epsRewatchedEnabled;
    wxCheckBox* m_totalEpsEnabled;
    wxCheckBox* m_lengthEnabled;
    wxCheckBox* m_dateStartedEnabled;
    wxCheckBox* m_dateFinishedEnabled;
    wxCheckBox* m_tagKeyEnabled;
    wxCheckBox* m_tagValEnabled;

    wxCheckBox* m_tagKeyInverse;
    wxCheckBox* m_tagValInverse;

    wxSpinCtrl* m_ratingsSpinLow;
    wxSpinCtrl* m_ratingsSpinHigh;
    wxSpinCtrl* m_yearSpinLow;
    wxSpinCtrl* m_yearSpinHigh;
    wxSpinCtrl* m_epsWatchedSpinLow;
    wxSpinCtrl* m_epsWatchedSpinHigh;
    wxSpinCtrl* m_epsRewatchedSpinLow;
    wxSpinCtrl* m_epsRewatchedSpinHigh;
    wxSpinCtrl* m_totalEpsSpinLow;
    wxSpinCtrl* m_totalEpsSpinHigh;
    wxSpinCtrl* m_lengthSpinLow;
    wxSpinCtrl* m_lengthSpinHigh;

    wxSpinCtrl* m_startYearLow;
    wxSpinCtrl* m_startMonthLow;
    wxSpinCtrl* m_startDayLow;
    wxSpinCtrl* m_startYearHigh;
    wxSpinCtrl* m_startMonthHigh;
    wxSpinCtrl* m_startDayHigh;
    wxSpinCtrl* m_finishYearLow;
    wxSpinCtrl* m_finishMonthLow;
    wxSpinCtrl* m_finishDayLow;
    wxSpinCtrl* m_finishYearHigh;
    wxSpinCtrl* m_finishMonthHigh;
    wxSpinCtrl* m_finishDayHigh;

    wxDECLARE_EVENT_TABLE();
};

#endif
