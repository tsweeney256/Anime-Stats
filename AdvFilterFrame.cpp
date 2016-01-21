#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include "AdvFilterFrame.hpp"
#include "AppIDs.hpp"

BEGIN_EVENT_TABLE(AdvFilterFrame, wxFrame)
    EVT_CLOSE(AdvFilterFrame::OnClose)
    EVT_CHECKBOX(ID_ADV_FILTER_WATCHED_STATUS, AdvFilterFrame::OnWatchedStatusCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_WATCHED_STATUS_ALL, AdvFilterFrame::OnWatchedStatusAllCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_RELEASE_TYPE, AdvFilterFrame::OnReleaseTypeCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_RELEASE_TYPE_ALL, AdvFilterFrame::OnReleaseTypeAllCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_SEASON, AdvFilterFrame::OnSeasonCheckBox)
    EVT_CHECKBOX(ID_ADV_FILTER_SEASON_ALL, AdvFilterFrame::OnSeasonAllCheckBox)
    EVT_BUTTON(ID_ADV_FILTER_RESET, AdvFilterFrame::OnReset)
    EVT_BUTTON(ID_ADV_FILTER_CANCEL, AdvFilterFrame::OnCancel)
END_EVENT_TABLE()

AdvFilterFrame::AdvFilterFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, ID_ADV_FILTER_FRAME, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    ///
    ///
    ///AdvFilterFrame
    ///
    ///

    m_mainPanel = new wxPanel(this, wxID_ANY);

    //
    //checkboxes
    //

    m_watchedCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Watched"));
    m_watchingCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Watching"));
    m_stalledCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Stalled"));
    m_droppedCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Dropped"));
    m_watchedStatusBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS, _("Blank"));
    m_watchedStatusAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_WATCHED_STATUS_ALL, _("Enable All"));
    m_tvCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("TV"));
    m_ovaCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("OVA"));
    m_onaCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("ONA"));
    m_movieCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("Movie"));
    m_tvSpecialCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("TV Special"));
    m_releaseTypeBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE, _("Blank"));
    m_releaseTypeAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_RELEASE_TYPE_ALL, _("Enable All"));
    m_winterCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Winter"));
    m_springCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Spring"));
    m_summerCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Summer"));
    m_fallCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Fall"));
    m_seasonBlankCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON, _("Blank"));
    m_seasonAllCheck = new wxCheckBox(m_mainPanel, ID_ADV_FILTER_SEASON_ALL, _("Enable All"));

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

    //
    //textctrl
    //

    m_titleTextField = new wxTextCtrl(m_mainPanel, wxID_ANY);

    //
    //spinCtrls
    //

    //for determining the width of the text controls
    //times 1000 to give 3 extra characters of padding because
    //GetSizeFromTextSize() doesn't work right on fedora and probably gtk3 in general
    const wxString maxNumStr   = wxString::Format(wxT("%i"), m_maxNum * 1000);
    const wxString maxYearStr  = wxString::Format(wxT("%i"), m_maxYear * 1000);
    const wxString maxMonthDayStr = wxString::Format(wxT("%i"), m_maxMonth * 1000);

    wxSize maxNumTextSize;
    wxSize maxYearTextSize;
    wxSize maxMonthDayTextSize;

    m_ratingsSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    //find max size needed for the textbox, then set it to that
    maxNumTextSize = m_ratingsSpinLow->GetSizeFromTextSize(m_ratingsSpinLow->GetTextExtent(maxNumStr));
    m_ratingsSpinLow->SetInitialSize(maxNumTextSize);

    m_ratingsSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);
    m_yearSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_yearSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);
    m_epsWatchedSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_epsWatchedSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);
    m_rewatchedSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_rewatchedSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);
    m_totalEpsSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_totalEpsSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);
    m_lengthSpinLow = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_minNum);
    m_lengthSpinHigh = new wxSpinCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, maxNumTextSize,
            wxSP_ARROW_KEYS, m_minNum, m_maxNum, m_maxNum);

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

    //
    //buttons
    //

    auto okButton = new wxButton(m_mainPanel, ID_ADV_FILTER_OK, "OK");
    auto cancelButton = new wxButton(m_mainPanel, ID_ADV_FILTER_CANCEL, "Cancel");
    auto applyButton = new wxButton(m_mainPanel, ID_ADV_FILTER_APPLY, "Apply");
    auto resetButton = new wxButton(m_mainPanel, ID_ADV_FILTER_RESET, "Reset");

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
    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    auto leftSizer = new wxBoxSizer(wxVERTICAL);
    auto midSizer = new wxBoxSizer(wxVERTICAL);
    auto rightSizer = new wxBoxSizer(wxVERTICAL);

    auto watchedStatusSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Watched Status"));
    auto releaseTypeSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Release Type"));
    auto seasonSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Season"));
    auto textFieldSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Title"));
    auto ratingsSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Rating"));
    auto yearSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Year Released"));
    auto epsWatchedSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Episodes Watched"));
    auto rewatchedSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Episodes Rewatched"));
    auto totalEpsSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Total Episodes"));
    auto lengthSizerOutline = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Episode Length"));
    auto dateStartedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Date Started (YYYY-MM-DD)"));
    auto dateFinishedSizerOutline = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Date Finished (YYYY-MM-DD)"));

    auto watchedStatusSizer = new wxGridSizer(3, 5, 5);
    auto releaseTypeSizer = new wxGridSizer(3, 5, 5);
    auto seasonSizer = new wxGridSizer(3, 5, 5);
    auto ratingsSizer = new wxBoxSizer(wxHORIZONTAL);
    auto yearSizer = new wxBoxSizer(wxHORIZONTAL);
    auto epsWatchedSizer = new wxBoxSizer(wxHORIZONTAL);
    auto rewatchedSizer = new wxBoxSizer(wxHORIZONTAL);
    auto totalEpsSizer = new wxBoxSizer(wxHORIZONTAL);
    auto lengthSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedLowSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedMidSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateStartedHighSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedLowSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedMidSizer = new wxBoxSizer(wxHORIZONTAL);
    auto dateFinishedHighSizer = new wxBoxSizer(wxHORIZONTAL);

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

    ratingsSizer->Add(m_ratingsSpinLow, paddingFlag);
    ratingsSizer->Add(getLabel(throughStr), paddingFlag);
    ratingsSizer->Add(m_ratingsSpinHigh, paddingFlag);

    yearSizer->Add(m_yearSpinLow, paddingFlag);
    yearSizer->Add(getLabel(throughStr), paddingFlag);
    yearSizer->Add(m_yearSpinHigh, paddingFlag);

    epsWatchedSizer->Add(m_epsWatchedSpinLow, paddingFlag);
    epsWatchedSizer->Add(getLabel(throughStr), paddingFlag);
    epsWatchedSizer->Add(m_epsWatchedSpinHigh, paddingFlag);

    rewatchedSizer->Add(m_rewatchedSpinLow, paddingFlag);
    rewatchedSizer->Add(getLabel(throughStr), paddingFlag);
    rewatchedSizer->Add(m_rewatchedSpinHigh, paddingFlag);

    totalEpsSizer->Add(m_totalEpsSpinLow, paddingFlag);
    totalEpsSizer->Add(getLabel(throughStr), paddingFlag);
    totalEpsSizer->Add(m_totalEpsSpinHigh, paddingFlag);

    lengthSizer->Add(m_lengthSpinLow, paddingFlag);
    lengthSizer->Add(getLabel(throughStr), paddingFlag);
    lengthSizer->Add(m_lengthSpinHigh, paddingFlag);

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

    watchedStatusSizerOutline->Add(watchedStatusSizer, paddingFlag);
    releaseTypeSizerOutline->Add(releaseTypeSizer, paddingFlag);
    seasonSizerOutline->Add(seasonSizer, paddingFlag);
    textFieldSizerOutline->Add(m_titleTextField, wxSizerFlags(1).Border(wxALL).Expand());
    ratingsSizerOutline->Add(ratingsSizer, paddingFlag);
    yearSizerOutline->Add(yearSizer, paddingFlag);
    epsWatchedSizerOutline->Add(epsWatchedSizer, paddingFlag);
    rewatchedSizerOutline->Add(rewatchedSizer, paddingFlag);
    totalEpsSizerOutline->Add(totalEpsSizer, paddingFlag);
    lengthSizerOutline->Add(lengthSizer, paddingFlag);
    dateStartedSizerOutline->Add(dateStartedLowSizer, paddingFlag);
    dateStartedSizerOutline->Add(dateStartedMidSizer, paddingCenterFlag);
    dateStartedSizerOutline->Add(dateStartedHighSizer, paddingFlag);
    dateFinishedSizerOutline->Add(dateFinishedLowSizer, paddingFlag);
    dateFinishedSizerOutline->Add(dateFinishedMidSizer, paddingCenterFlag);
    dateFinishedSizerOutline->Add(dateFinishedHighSizer, paddingFlag);

    leftSizer->Add(watchedStatusSizerOutline, expandFlag);
    leftSizer->Add(releaseTypeSizerOutline, expandFlag);
    leftSizer->Add(seasonSizerOutline, expandFlag);
    leftSizer->Add(textFieldSizerOutline, expandFlag);

    midSizer->Add(ratingsSizerOutline, expandFlag);
    midSizer->Add(yearSizerOutline, expandFlag);
    midSizer->Add(epsWatchedSizerOutline, expandFlag);
    midSizer->Add(rewatchedSizerOutline, expandFlag);
    midSizer->Add(totalEpsSizerOutline, expandFlag);
    midSizer->Add(lengthSizerOutline, expandFlag);

    rightSizer->Add(dateStartedSizerOutline, expandFlag);
    rightSizer->Add(dateFinishedSizerOutline, expandFlag);

    contentSizer->Add(leftSizer, paddingFlag);
    contentSizer->Add(midSizer, paddingFlag);
    contentSizer->Add(rightSizer, paddingFlag);

    buttonSizer->Add(okButton, paddingFlag);
    buttonSizer->Add(cancelButton, paddingFlag);
    buttonSizer->Add(applyButton, paddingFlag);
    buttonSizer->Add(resetButton, paddingFlag);

    mainSizer->Add(contentSizer, paddingFlag);
    mainSizer->Add(buttonSizer, paddingRightFlag);

    m_mainPanel->SetSizerAndFit(mainSizer);
    this->Fit();
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

void AdvFilterFrame::OnReset(wxCommandEvent& WXUNUSED(event))
{
    EnableAllWatchedStatus();
    EnableAllReleaseType();
    EnableAllSeason();

    m_watchedStatusAllCheck->SetValue(true);
    m_releaseTypeAllCheck->SetValue(true);
    m_seasonAllCheck->SetValue(true);

    m_titleTextField->SetValue("");

    m_ratingsSpinLow->SetValue(m_minNum);
    m_ratingsSpinHigh->SetValue(m_maxNum);
    m_yearSpinLow->SetValue(m_minNum);
    m_yearSpinHigh->SetValue(m_maxNum);
    m_epsWatchedSpinLow->SetValue(m_minNum);
    m_epsWatchedSpinHigh->SetValue(m_maxNum);
    m_rewatchedSpinLow->SetValue(m_minNum);
    m_rewatchedSpinHigh->SetValue(m_maxNum);
    m_totalEpsSpinLow->SetValue(m_minNum);
    m_totalEpsSpinHigh->SetValue(m_maxNum);
    m_lengthSpinLow->SetValue(m_minNum);
    m_lengthSpinHigh->SetValue(m_maxNum);

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
}

void AdvFilterFrame::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    Close();
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
