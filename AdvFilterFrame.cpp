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

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include "AdvFilterFrame.hpp"
#include "DataPanel.hpp"
#include "FilterStructs.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvFilterFrame, wxFrame)
    EVT_CLOSE(AdvFilterFrame::OnClose)
    EVT_CHECKBOX(ID_ADV_FILTER_WATCHED_STATUS, AdvFilterFrame::OnWatchedStatusCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_WATCHED_STATUS_ALL, AdvFilterFrame::OnWatchedStatusAllCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_RELEASE_TYPE, AdvFilterFrame::OnReleaseTypeCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_RELEASE_TYPE_ALL, AdvFilterFrame::OnReleaseTypeAllCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_SEASON, AdvFilterFrame::OnSeasonCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_SEASON_ALL, AdvFilterFrame::OnSeasonAllCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_RATING_ENABLE, AdvFilterFrame::OnRatingEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_YEAR_ENABLE, AdvFilterFrame::OnYearEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_EPS_WATCHED_ENABLE, AdvFilterFrame::OnEpsWatchedEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_EPS_REWATCHED_ENABLE, AdvFilterFrame::OnEpsRewatchedEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_TOTAL_EPS_ENABLE, AdvFilterFrame::OnTotalEpsEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_LENGTH_ENABLE, AdvFilterFrame::OnLengthEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_DATE_STARTED_ENABLE, AdvFilterFrame::OnDateStartedEnableCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_DATE_FINISHED_ENABLE, AdvFilterFrame::OnDateFinishedEnableCheckBox)
    EVT_BUTTON(wxID_HELP, AdvFilterFrame::OnReset)
    EVT_BUTTON(wxID_CANCEL, AdvFilterFrame::OnCancel)
    EVT_BUTTON(wxID_OK, AdvFilterFrame::OnOK)
    EVT_BUTTON(wxID_APPLY, AdvFilterFrame::OnApply)
END_EVENT_TABLE()

AdvFilterFrame::AdvFilterFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, ID_ADV_FILTER_FRAME, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(/*wxRESIZE_BORDER |*/ wxMAXIMIZE_BOX))
{
    m_parent = dynamic_cast<DataPanel*>(parent);
    wxASSERT(parent);
    m_mainPanel = new wxScrolledWindow(this, wxID_ANY);
    m_mainPanel->SetScrollRate(10, 10);

    m_titleTextField = new wxTextCtrl(m_mainPanel, wxID_ANY);
    m_studioTextField = new wxTextCtrl(m_mainPanel, wxID_ANY);
    
    m_watchedCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Watched"));
    m_watchingCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Watching"));
    m_stalledCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Stalled"));
    m_droppedCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Dropped"));
    m_watchedStatusBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Blank"));
    m_watchedStatusAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS_ALL, _("Enable All"));
    m_tvCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("TV"));
    m_movieCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("Movie"));
    m_ovaCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("OVA"));
    m_onaCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("ONA"));
    m_tvSpecialCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("TV Special"));
    m_releaseTypeBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("Blank"));
    m_releaseTypeAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE_ALL, _("Enable All"));
    m_winterCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Winter"));
    m_springCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Spring"));
    m_summerCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Summer"));
    m_fallCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Fall"));
    m_seasonBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Blank"));
    m_seasonAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON_ALL, _("Enable All"));

    //for determining the width of the text controls
    //times 1000 to give 3 extra characters of padding because
    //GetSizeFromTextSize() doesn't work right on fedora and probably gtk3 in general
    const wxString maxNumStr   = wxString::Format(wxT("%i"), m_maxNum * 1000);
    const wxString maxYearStr  = wxString::Format(wxT("%i"), m_maxYear * 1000);
    const wxString maxMonthDayStr = wxString::Format(wxT("%i"), m_maxMonth * 1000);

    wxSize maxNumTextSize;
    wxSize maxYearTextSize;
    wxSize maxMonthDayTextSize;

    m_ratingsEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RATING_ENABLE, _("Enable"));
    m_ratingsSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    //find max size needed for the textbox, then set it to that
    maxNumTextSize = m_ratingsSpinLow->GetSizeFromTextSize(m_ratingsSpinLow->GetTextExtent(maxNumStr));
    m_ratingsSpinLow->SetInitialSize(maxNumTextSize);

    m_ratingsSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                       wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_yearEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_YEAR_ENABLE, _("Enable"));
    m_yearSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                   wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_yearSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                    wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_epsWatchedEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_EPS_WATCHED_ENABLE, _("Enable"));
    m_epsWatchedSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                         wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_epsWatchedSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                          wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_epsRewatchedEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_EPS_REWATCHED_ENABLE, _("Enable"));
    m_epsRewatchedSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                           wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_epsRewatchedSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_totalEpsEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_TOTAL_EPS_ENABLE, _("Enable"));
    m_totalEpsSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                       wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_totalEpsSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                        wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_lengthEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_LENGTH_ENABLE, _("Enable"));
    m_lengthSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                     wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_lengthSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
                                      wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

    m_dateStartedEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_DATE_STARTED_ENABLE, _("Enable"));
    m_startYearLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    wxSP_ARROW_KEYS, m_minYear, m_maxYear, m_minYear);
    maxYearTextSize = m_startYearLow->GetSizeFromTextSize(m_startYearLow->GetTextExtent(maxYearStr));
    m_startYearLow->SetInitialSize(maxYearTextSize);
    m_startMonthLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                     wxSP_ARROW_KEYS, m_minMonth, m_maxMonth, m_minMonth);
    maxMonthDayTextSize = m_startMonthLow->GetSizeFromTextSize(m_startMonthLow->GetTextExtent(maxMonthDayStr));
    m_startMonthLow->SetInitialSize(maxMonthDayTextSize);
    m_startDayLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                   wxSP_ARROW_KEYS, m_minDay, m_maxDay, m_minDay);
    m_startYearHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxYearTextSize,
                                     wxSP_ARROW_KEYS, m_minYear, m_maxYear, m_maxYear);
    m_startMonthHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                      wxSP_ARROW_KEYS, m_minMonth, m_maxMonth, m_maxMonth);
    m_startDayHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                    wxSP_ARROW_KEYS, m_minDay, m_maxDay, m_maxDay);

    m_dateFinishedEnabled = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_DATE_FINISHED_ENABLE, _("Enable"));
    m_finishYearLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxYearTextSize,
                                     wxSP_ARROW_KEYS, m_minYear, m_maxYear, m_minYear);
    m_finishMonthLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                      wxSP_ARROW_KEYS, m_minMonth, m_maxMonth, m_minMonth);
    m_finishDayLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                    wxSP_ARROW_KEYS, m_minDay, m_maxDay, m_minDay);
    m_finishYearHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxYearTextSize,
                                      wxSP_ARROW_KEYS, m_minYear, m_maxYear, m_maxYear);
    m_finishMonthHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                       wxSP_ARROW_KEYS, m_minMonth, m_maxMonth, m_maxMonth);
    m_finishDayHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxMonthDayTextSize,
                                     wxSP_ARROW_KEYS, m_minDay, m_maxDay, m_maxDay);

    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_watchedStatusBlankCheck->SetValue(true);
    m_watchedStatusAllCheck->SetValue(true);
    m_watchedStatusAllCheck->Disable();
    m_tvCheck->SetValue(true);
    m_ovaCheck->SetValue(true);
    m_onaCheck->SetValue(true);
    m_movieCheck->SetValue(true);
    m_tvSpecialCheck->SetValue(true);
    m_releaseTypeBlankCheck->SetValue(true);
    m_releaseTypeAllCheck->SetValue(true);
    m_releaseTypeAllCheck->Disable();
    m_winterCheck->SetValue(true);
    m_springCheck->SetValue(true);
    m_summerCheck->SetValue(true);
    m_fallCheck->SetValue(true);
    m_seasonBlankCheck->SetValue(true);
    m_seasonAllCheck->SetValue(true);
    m_seasonAllCheck->Disable();

    //all spin controls are disable until explicitly enabled by the user
    m_ratingsSpinLow->Disable();
    m_ratingsSpinHigh->Disable();
    m_yearSpinLow->Disable();
    m_yearSpinHigh->Disable();
    m_epsWatchedSpinLow->Disable();
    m_epsWatchedSpinHigh->Disable();
    m_epsRewatchedSpinLow->Disable();
    m_epsRewatchedSpinHigh->Disable();
    m_totalEpsSpinLow->Disable();
    m_totalEpsSpinHigh->Disable();
    m_lengthSpinLow->Disable();
    m_lengthSpinHigh->Disable();
    m_startYearLow->Disable();
    m_startMonthLow->Disable();
    m_startDayLow->Disable();
    m_startYearHigh->Disable();
    m_startMonthHigh->Disable();
    m_startDayHigh->Disable();
    m_finishYearLow->Disable();
    m_finishMonthLow->Disable();
    m_finishDayLow->Disable();
    m_finishYearHigh->Disable();
    m_finishMonthHigh->Disable();
    m_finishDayHigh->Disable();

    auto okButton = new wxButton(m_mainPanel, wxID_OK, "OK");
    auto cancelButton = new wxButton(m_mainPanel, wxID_CANCEL, "Cancel");
    auto applyButton = new wxButton(m_mainPanel, wxID_APPLY, "Apply");
    auto resetButton = new wxButton(m_mainPanel, wxID_HELP, "Reset"); //wxID_HELP as a heck to get it to work with the sizer

    //
    //sizers
    //

    const auto throughStr = _("through");
    const auto hyphenStr = _("-");

    auto paddingFlag = wxSizerFlags().Border(wxALL);
    auto paddingCenterFlag = wxSizerFlags().Border(wxALL).Center();
    auto paddingRightFlag = wxSizerFlags().Border(wxALL).Right();
    auto expandFlag = wxSizerFlags().Border(wxALL).Expand();

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto contentSizer = new wxBoxSizer(wxHORIZONTAL);
    auto osButtonSizer = new wxStdDialogButtonSizer();
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    auto midSizer = new wxBoxSizer(wxVERTICAL);
    auto rightSizer = new wxBoxSizer(wxVERTICAL);

    auto watchedStatusSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Watched Status"));
    auto releaseTypeSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Release Type"));
    auto seasonSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Season"));
    auto titleFieldSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Title"));
    auto studioFieldSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Studio"));
    auto ratingsSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Rating"));
    auto yearSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Year Released"));
    auto epsWatchedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Episodes Watched"));
    auto rewatchedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Episodes Rewatched"));
    auto totalEpsSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Total Episodes"));
    auto lengthSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Episode Length"));
    auto dateStartedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Date Started (YYYY-MM-DD)"));
    auto dateFinishedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Date Finished (YYYY-MM-DD)"));

    auto watchedStatusSizer = new wxGridSizer(3, 5, 5);
    auto releaseTypeSizer = new wxGridSizer(3, 5, 5);
    auto seasonSizer = new wxGridSizer(3, 5, 5);
    auto ratingsBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto yearBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto epsWatchedBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto rewatchedBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto totalEpsBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto lengthBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    auto ratingsTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto yearTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto epsWatchedTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto rewatchedTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto totalEpsTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto lengthTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedLowSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedMidSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedHighSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedTopSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedLowSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedMidSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedHighSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedTopSizer = new wxBoxSizer(wxHORIZONTAL);

    watchedStatusSizer->Add(m_watchedCheck, paddingFlag);
    watchedStatusSizer->Add(m_stalledCheck, paddingFlag);
    watchedStatusSizer->Add(m_watchedStatusBlankCheck, paddingFlag);
    watchedStatusSizer->Add(m_watchingCheck, paddingFlag);
    watchedStatusSizer->Add(m_droppedCheck, paddingFlag);
    watchedStatusSizer->Add(m_watchedStatusAllCheck, paddingFlag);

    releaseTypeSizer->Add(m_tvCheck, paddingFlag);
    releaseTypeSizer->Add(m_ovaCheck, paddingFlag);
    releaseTypeSizer->Add(m_tvSpecialCheck, paddingFlag);
    releaseTypeSizer->Add(m_movieCheck, paddingFlag);
    releaseTypeSizer->Add(m_onaCheck, paddingFlag);
    releaseTypeSizer->Add(m_releaseTypeBlankCheck, paddingFlag);
    releaseTypeSizer->Add(m_releaseTypeAllCheck, paddingFlag);

    seasonSizer->Add(m_winterCheck, paddingFlag);
    seasonSizer->Add(m_summerCheck, paddingFlag);
    seasonSizer->Add(m_seasonBlankCheck, paddingFlag);
    seasonSizer->Add(m_springCheck, paddingFlag);
    seasonSizer->Add(m_fallCheck, paddingFlag);
    seasonSizer->Add(m_seasonAllCheck, paddingFlag);

    ratingsTopSizer->Add(m_ratingsEnabled, paddingFlag);
    ratingsBottomSizer->Add(m_ratingsSpinLow, paddingFlag);
    ratingsBottomSizer->Add(getLabel(throughStr), paddingFlag);
    ratingsBottomSizer->Add(m_ratingsSpinHigh, paddingFlag);

    yearTopSizer->Add(m_yearEnabled, paddingFlag);
    yearBottomSizer->Add(m_yearSpinLow, paddingFlag);
    yearBottomSizer->Add(getLabel(throughStr), paddingFlag);
    yearBottomSizer->Add(m_yearSpinHigh, paddingFlag);

    epsWatchedTopSizer->Add(m_epsWatchedEnabled, paddingFlag);
    epsWatchedBottomSizer->Add(m_epsWatchedSpinLow, paddingFlag);
    epsWatchedBottomSizer->Add(getLabel(throughStr), paddingFlag);
    epsWatchedBottomSizer->Add(m_epsWatchedSpinHigh, paddingFlag);

    rewatchedTopSizer->Add(m_epsRewatchedEnabled, paddingFlag);
    rewatchedBottomSizer->Add(m_epsRewatchedSpinLow, paddingFlag);
    rewatchedBottomSizer->Add(getLabel(throughStr), paddingFlag);
    rewatchedBottomSizer->Add(m_epsRewatchedSpinHigh, paddingFlag);

    totalEpsTopSizer->Add(m_totalEpsEnabled, paddingFlag);
    totalEpsBottomSizer->Add(m_totalEpsSpinLow, paddingFlag);
    totalEpsBottomSizer->Add(getLabel(throughStr), paddingFlag);
    totalEpsBottomSizer->Add(m_totalEpsSpinHigh, paddingFlag);

    lengthTopSizer->Add(m_lengthEnabled, paddingFlag);
    lengthBottomSizer->Add(m_lengthSpinLow, paddingFlag);
    lengthBottomSizer->Add(getLabel(throughStr), paddingFlag);
    lengthBottomSizer->Add(m_lengthSpinHigh, paddingFlag);

    dateStartedLowSizer->Add(m_startYearLow, paddingFlag);
    dateStartedLowSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateStartedLowSizer->Add(m_startMonthLow, paddingFlag);
    dateStartedLowSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateStartedLowSizer->Add(m_startDayLow, paddingFlag);
    dateStartedMidSizer->Add(getLabel(throughStr), paddingFlag);
    dateStartedHighSizer->Add(m_startYearHigh, paddingFlag);
    dateStartedHighSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateStartedHighSizer->Add(m_startMonthHigh, paddingFlag);
    dateStartedHighSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateStartedHighSizer->Add(m_startDayHigh, paddingFlag);
    dateStartedTopSizer->Add(m_dateStartedEnabled, paddingFlag);

    dateFinishedLowSizer->Add(m_finishYearLow, paddingFlag);
    dateFinishedLowSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateFinishedLowSizer->Add(m_finishMonthLow, paddingFlag);
    dateFinishedLowSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateFinishedLowSizer->Add(m_finishDayLow, paddingFlag);
    dateFinishedMidSizer->Add(getLabel(throughStr), paddingFlag);
    dateFinishedHighSizer->Add(m_finishYearHigh, paddingFlag);
    dateFinishedHighSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateFinishedHighSizer->Add(m_finishMonthHigh, paddingFlag);
    dateFinishedHighSizer->Add(getLabel(hyphenStr), paddingFlag);
    dateFinishedHighSizer->Add(m_finishDayHigh, paddingFlag);
    dateFinishedTopSizer->Add(m_dateFinishedEnabled, paddingFlag);

    watchedStatusSizerOutline->Add(watchedStatusSizer, paddingFlag);
    releaseTypeSizerOutline->Add(releaseTypeSizer, paddingFlag);
    seasonSizerOutline->Add(seasonSizer, paddingFlag);
    titleFieldSizerOutline->Add(m_titleTextField, wxSizerFlags(1).Border(wxALL).Expand());
    studioFieldSizerOutline->Add(m_studioTextField, wxSizerFlags(1).Border(wxALL).Expand());

    ratingsSizerOutline->Add(ratingsTopSizer, paddingFlag);
    yearSizerOutline->Add(yearTopSizer, paddingFlag);
    epsWatchedSizerOutline->Add(epsWatchedTopSizer, paddingFlag);
    rewatchedSizerOutline->Add(rewatchedTopSizer, paddingFlag);
    totalEpsSizerOutline->Add(totalEpsTopSizer, paddingFlag);
    lengthSizerOutline->Add(lengthTopSizer, paddingFlag);
    ratingsSizerOutline->Add(ratingsBottomSizer, paddingFlag);
    yearSizerOutline->Add(yearBottomSizer, paddingFlag);
    epsWatchedSizerOutline->Add(epsWatchedBottomSizer, paddingFlag);
    rewatchedSizerOutline->Add(rewatchedBottomSizer, paddingFlag);
    totalEpsSizerOutline->Add(totalEpsBottomSizer, paddingFlag);
    lengthSizerOutline->Add(lengthBottomSizer, paddingFlag);

    dateStartedSizerOutline->Add(dateStartedTopSizer, paddingFlag);
    dateStartedSizerOutline->Add(dateStartedLowSizer, paddingFlag);
    dateStartedSizerOutline->Add(dateStartedMidSizer, paddingCenterFlag);
    dateStartedSizerOutline->Add(dateStartedHighSizer, paddingFlag);
    dateFinishedSizerOutline->Add(dateFinishedTopSizer, paddingFlag);
    dateFinishedSizerOutline->Add(dateFinishedLowSizer, paddingFlag);
    dateFinishedSizerOutline->Add(dateFinishedMidSizer, paddingCenterFlag);
    dateFinishedSizerOutline->Add(dateFinishedHighSizer, paddingFlag);

    leftSizer->Add(titleFieldSizerOutline, expandFlag);
    leftSizer->Add(watchedStatusSizerOutline, expandFlag);
    leftSizer->Add(releaseTypeSizerOutline, expandFlag);
    leftSizer->Add(seasonSizerOutline, expandFlag);
    leftSizer->Add(ratingsSizerOutline, expandFlag);

    midSizer->Add(studioFieldSizerOutline, expandFlag);
    midSizer->Add(yearSizerOutline, expandFlag);
    midSizer->Add(epsWatchedSizerOutline, expandFlag);
    midSizer->Add(rewatchedSizerOutline, expandFlag);
    midSizer->Add(totalEpsSizerOutline, expandFlag);

    rightSizer->Add(lengthSizerOutline, expandFlag);
    rightSizer->Add(dateStartedSizerOutline, expandFlag);
    rightSizer->Add(dateFinishedSizerOutline, expandFlag);

    contentSizer->Add(leftSizer, paddingFlag);
    contentSizer->Add(midSizer, paddingFlag);
    contentSizer->Add(rightSizer, paddingFlag);

    osButtonSizer->AddButton(okButton);
    osButtonSizer->AddButton(cancelButton);
    osButtonSizer->AddButton(applyButton);
    osButtonSizer->AddButton(resetButton);
    osButtonSizer->Realize();
    buttonSizer->Add(osButtonSizer, paddingFlag);
    //buttonSizer->Add(resetButton, paddingFlag);

    mainSizer->Add(contentSizer, paddingFlag);
    mainSizer->Add(buttonSizer, paddingRightFlag);

    m_mainPanel->SetSizerAndFit(mainSizer);
    this->Fit();
    this->SetMaxSize(this->GetSize());
}

void AdvFilterFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void AdvFilterFrame::OnWatchedStatusCheckBox(wxCommandEvent& event)
{
    if(!event.GetInt()){
        m_watchedStatusAllCheck->SetValue(false);
        m_watchedStatusAllCheck->Enable();
    }
    else{
        if(m_watchedCheck->GetValue() &&
           m_watchingCheck->GetValue() &&
           m_stalledCheck->GetValue() &&
           m_droppedCheck->GetValue() &&
           m_watchedStatusBlankCheck->GetValue()){
            m_watchedStatusAllCheck->SetValue(true);
            m_watchedStatusAllCheck->Disable();
        }
    }
}

void AdvFilterFrame::OnWatchedStatusAllCheckBox(wxCommandEvent& WXUNUSED(event))
{
    EnableAllWatchedStatus();
}

void AdvFilterFrame::OnReleaseTypeCheckBox(wxCommandEvent& event)
{
    if(!event.GetInt()){
        m_releaseTypeAllCheck->SetValue(false);
        m_releaseTypeAllCheck->Enable();
    }
    else{
        if(m_tvCheck->GetValue() &&
           m_ovaCheck->GetValue() &&
           m_onaCheck->GetValue() &&
           m_movieCheck->GetValue() &&
           m_tvSpecialCheck->GetValue() &&
           m_releaseTypeBlankCheck->GetValue()){
            m_releaseTypeAllCheck->SetValue(true);
            m_releaseTypeAllCheck->Disable();
        }
    }
}

void AdvFilterFrame::OnReleaseTypeAllCheckBox(wxCommandEvent& WXUNUSED(event))
{
    EnableAllReleaseType();
}

void AdvFilterFrame::OnSeasonCheckBox(wxCommandEvent& event)
{
    if(!event.GetInt()){
        m_seasonAllCheck->SetValue(false);
        m_seasonAllCheck->Enable();
    }
    else{
        if(m_winterCheck->GetValue() &&
           m_springCheck->GetValue() &&
           m_summerCheck->GetValue() &&
           m_fallCheck->GetValue() &&
           m_seasonBlankCheck->GetValue()){
            m_seasonAllCheck->SetValue(true);
            m_seasonAllCheck->Disable();
        }
    }
}

void AdvFilterFrame::OnSeasonAllCheckBox(wxCommandEvent& WXUNUSED(event))
{
    EnableAllSeason();
}

void AdvFilterFrame::OnRatingEnableCheckBox(wxCommandEvent& event)
{
    m_ratingsSpinLow->Enable(event.GetInt());
    m_ratingsSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnYearEnableCheckBox(wxCommandEvent& event)
{
    m_yearSpinLow->Enable(event.GetInt());
    m_yearSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnEpsWatchedEnableCheckBox(wxCommandEvent& event)
{
    m_epsWatchedSpinLow->Enable(event.GetInt());
    m_epsWatchedSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnEpsRewatchedEnableCheckBox(wxCommandEvent& event)
{
    m_epsRewatchedSpinLow->Enable(event.GetInt());
    m_epsRewatchedSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnTotalEpsEnableCheckBox(wxCommandEvent& event)
{
    m_totalEpsSpinLow->Enable(event.GetInt());
    m_totalEpsSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnLengthEnableCheckBox(wxCommandEvent& event)
{
    m_lengthSpinLow->Enable(event.GetInt());
    m_lengthSpinHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnDateStartedEnableCheckBox(wxCommandEvent& event)
{
    m_startYearLow->Enable(event.GetInt());
    m_startMonthLow->Enable(event.GetInt());
    m_startDayLow->Enable(event.GetInt());
    m_startYearHigh->Enable(event.GetInt());
    m_startMonthHigh->Enable(event.GetInt());
    m_startDayHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnDateFinishedEnableCheckBox(wxCommandEvent& event)
{
    m_finishYearLow->Enable(event.GetInt());
    m_finishMonthLow->Enable(event.GetInt());
    m_finishDayLow->Enable(event.GetInt());
    m_finishYearHigh->Enable(event.GetInt());
    m_finishMonthHigh->Enable(event.GetInt());
    m_finishDayHigh->Enable(event.GetInt());
}

void AdvFilterFrame::OnReset(wxCommandEvent& WXUNUSED(event))
{
    EnableAllWatchedStatus();
    EnableAllReleaseType();
    EnableAllSeason();

    m_watchedStatusAllCheck->SetValue(true);
    m_releaseTypeAllCheck->SetValue(true);
    m_seasonAllCheck->SetValue(true);

    m_titleTextField->SetValue("");
    m_studioTextField->SetValue("");

    m_ratingsSpinLow->SetValue(m_minNum);
    m_ratingsSpinHigh->SetValue(m_maxNum);
    m_yearSpinLow->SetValue(m_minNum);
    m_yearSpinHigh->SetValue(m_maxNum);
    m_epsWatchedSpinLow->SetValue(m_minNum);
    m_epsWatchedSpinHigh->SetValue(m_maxNum);
    m_epsRewatchedSpinLow->SetValue(m_minNum);
    m_epsRewatchedSpinHigh->SetValue(m_maxNum);
    m_totalEpsSpinLow->SetValue(m_minNum);
    m_totalEpsSpinHigh->SetValue(m_maxNum);
    m_lengthSpinLow->SetValue(m_minNum);
    m_lengthSpinHigh->SetValue(m_maxNum);

    m_ratingsSpinLow->Disable();
    m_ratingsSpinHigh->Disable();
    m_yearSpinLow->Disable();
    m_yearSpinHigh->Disable();
    m_epsWatchedSpinLow->Disable();
    m_epsWatchedSpinHigh->Disable();
    m_epsRewatchedSpinLow->Disable();
    m_epsRewatchedSpinHigh->Disable();
    m_totalEpsSpinLow->Disable();
    m_totalEpsSpinHigh->Disable();
    m_lengthSpinLow->Disable();
    m_lengthSpinHigh->Disable();

    m_ratingsEnabled->SetValue(false);
    m_yearEnabled->SetValue(false);
    m_epsWatchedEnabled->SetValue(false);
    m_epsRewatchedEnabled->SetValue(false);
    m_totalEpsEnabled->SetValue(false);
    m_lengthEnabled->SetValue(false);

    m_startYearLow->SetValue(m_minYear);
    m_startMonthLow->SetValue(m_minMonth);
    m_startDayLow->SetValue(m_minDay);
    m_startYearHigh->SetValue(m_maxYear);
    m_startMonthHigh->SetValue(m_maxMonth);
    m_startDayHigh->SetValue(m_maxDay);
    m_finishYearLow->SetValue(m_minYear);
    m_finishMonthLow->SetValue(m_minMonth);
    m_finishDayLow->SetValue(m_minDay);
    m_finishYearHigh->SetValue(m_maxYear);
    m_finishMonthHigh->SetValue(m_maxMonth);
    m_finishDayHigh->SetValue(m_maxDay);

    m_startYearLow->Disable();
    m_startMonthLow->Disable();
    m_startDayLow->Disable();
    m_startYearHigh->Disable();
    m_startMonthHigh->Disable();
    m_startDayHigh->Disable();
    m_finishYearLow->Disable();
    m_finishMonthLow->Disable();
    m_finishDayLow->Disable();
    m_finishYearHigh->Disable();
    m_finishMonthHigh->Disable();
    m_finishDayHigh->Disable();

    m_dateStartedEnabled->SetValue(false);
    m_dateFinishedEnabled->SetValue(false);
}

void AdvFilterFrame::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void AdvFilterFrame::OnOK(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
    Close();
}

void AdvFilterFrame::OnApply(wxCommandEvent& WXUNUSED(event))
{
    ApplyFilter();
}

wxStaticText* AdvFilterFrame::getLabel(const wxString& text)
{
    return new wxStaticText(m_mainPanel, wxID_ANY, text);
}

void AdvFilterFrame::EnableAllWatchedStatus()
{
    m_watchedCheck->SetValue(true);
    m_watchingCheck->SetValue(true);
    m_stalledCheck->SetValue(true);
    m_droppedCheck->SetValue(true);
    m_watchedStatusBlankCheck->SetValue(true);
    m_watchedStatusAllCheck->Disable();
}

void AdvFilterFrame::EnableAllReleaseType()
{
    m_tvCheck->SetValue(true);
    m_ovaCheck->SetValue(true);
    m_onaCheck->SetValue(true);
    m_movieCheck->SetValue(true);
    m_tvSpecialCheck->SetValue(true);
    m_releaseTypeBlankCheck->SetValue(true);
    m_releaseTypeAllCheck->Disable();
}

void AdvFilterFrame::EnableAllSeason()
{
    m_winterCheck->SetValue(true);
    m_springCheck->SetValue(true);
    m_summerCheck->SetValue(true);
    m_fallCheck->SetValue(true);
    m_seasonBlankCheck->SetValue(true);
    m_seasonAllCheck->Disable();
}

void AdvFilterFrame::ApplyFilter()
{
    auto basic = BasicFilterInfo::MakeShared();
    auto adv = AdvFilterInfo::MakeShared();

    basic->title = m_titleTextField->GetValue().utf8_str();
    basic->watched = m_watchedCheck->GetValue();
    basic->watching = m_watchingCheck->GetValue();
    basic->stalled = m_stalledCheck->GetValue();
    basic->dropped = m_droppedCheck->GetValue();
    basic->watchedBlank = m_watchedStatusBlankCheck->GetValue();

    adv->studio = m_studioTextField->GetValue().utf8_str();
    
    adv->tv = m_tvCheck->GetValue();
    adv->ova = m_ovaCheck->GetValue();
    adv->ona = m_onaCheck->GetValue();
    adv->movie = m_movieCheck->GetValue();
    adv->tvSpecial = m_tvSpecialCheck->GetValue();
    adv->releaseBlank = m_releaseTypeBlankCheck->GetValue();

    adv->winter = m_winterCheck->GetValue();
    adv->spring = m_springCheck->GetValue();
    adv->summer = m_summerCheck->GetValue();
    adv->fall = m_fallCheck->GetValue();
    adv->seasonBlank = m_seasonBlankCheck->GetValue();

    adv->ratingEnabled = m_ratingsEnabled->GetValue();
    adv->yearEnabled = m_yearEnabled->GetValue();
    adv->epsWatchedEnabled = m_epsWatchedEnabled->GetValue();
    adv->epsRewatchedEnabled = m_epsRewatchedEnabled->GetValue();
    adv->totalEpsEnabled = m_totalEpsEnabled->GetValue();
    adv->lengthEnabled = m_lengthEnabled->GetValue();
    adv->dateStartedEnabled = m_dateStartedEnabled->GetValue();
    adv->dateFinishedEnabled = m_dateFinishedEnabled->GetValue();

    adv->ratingLow = m_ratingsSpinLow->GetValue();
    adv->yearLow = m_yearSpinLow->GetValue();
    adv->epsWatchedLow = m_epsWatchedSpinLow->GetValue();
    adv->epsRewatchedLow = m_epsRewatchedSpinLow->GetValue();
    adv->totalEpsLow = m_totalEpsSpinLow->GetValue();
    adv->lengthLow = m_lengthSpinLow->GetValue();
    adv->ratingHigh = m_ratingsSpinHigh->GetValue();
    adv->yearHigh = m_yearSpinHigh->GetValue();
    adv->epsWatchedHigh = m_epsWatchedSpinHigh->GetValue();
    adv->epsRewatchedHigh = m_epsRewatchedSpinHigh->GetValue();
    adv->totalEpsHigh = m_totalEpsSpinHigh->GetValue();
    adv->lengthHigh = m_lengthSpinHigh->GetValue();

    adv->yearStartedLow = m_startYearLow->GetValue();
    adv->monthStartedLow = m_startMonthLow->GetValue();
    adv->dayStartedLow = m_startDayLow->GetValue();
    adv->yearStartedHigh = m_startYearHigh->GetValue();
    adv->monthStartedHigh = m_startMonthHigh->GetValue();
    adv->dayStartedHigh = m_startDayHigh->GetValue();

    adv->yearFinishedLow = m_finishYearLow->GetValue();
    adv->monthFinishedLow = m_finishMonthLow->GetValue();
    adv->dayFinishedLow = m_finishDayLow->GetValue();
    adv->yearFinishedHigh = m_finishYearHigh->GetValue();
    adv->monthFinishedHigh = m_finishMonthHigh->GetValue();
    adv->dayFinishedHigh = m_finishDayHigh->GetValue();

    m_parent->NewFilter(basic, adv);
}
