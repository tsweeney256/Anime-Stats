#ifndef ADVFILTERFRAME_HPP
#define ADVFILTERFRAME_HPP

#include <wx/frame.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

class DataPanel;

class AdvFilterFrame : public wxFrame
{
public:
    AdvFilterFrame(wxWindow* parent, const wxString& title, const wxPoint &pos, const wxSize& size);

private:
    void OnClose(wxCloseEvent& event);
    void OnWatchedStatusCheckBox(wxCommandEvent& event);
    void OnWatchedStatusAllCheckBox(wxCommandEvent& event);
    void OnReleaseTypeCheckBox(wxCommandEvent& event);
    void OnReleaseTypeAllCheckBox(wxCommandEvent& event);
    void OnSeasonCheckBox(wxCommandEvent& event);
    void OnSeasonAllCheckBox(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);

    wxStaticText* getLabel(const wxString& text);
    void EnableAllWatchedStatus();
    void EnableAllReleaseType();
    void EnableAllSeason();
    void ApplyFilter();

    const int m_minNum    = 0;
    const int m_maxNum    = 999999;
    const int m_minYear   = 0;
    const int m_maxYear   = 9999;
    const int m_minMonth  = 1;
    const int m_maxMonth  = 12;
    const int m_minDay    = 1;
    const int m_maxDay    = 31;

    DataPanel* m_parent;
    wxPanel* m_mainPanel;

    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_watchedStatusBlankCheck;
    wxCheckBox* m_watchedStatusAllCheck;
    wxCheckBox* m_tvCheck;
    wxCheckBox* m_ovaCheck;
    wxCheckBox* m_onaCheck;
    wxCheckBox* m_movieCheck;
    wxCheckBox* m_tvSpecialCheck;
    wxCheckBox* m_releaseTypeBlankCheck;
    wxCheckBox* m_releaseTypeAllCheck;
    wxCheckBox* m_winterCheck;
    wxCheckBox* m_springCheck;
    wxCheckBox* m_summerCheck;
    wxCheckBox* m_fallCheck;
    wxCheckBox* m_seasonBlankCheck;
    wxCheckBox* m_seasonAllCheck;

    wxTextCtrl* m_titleTextField;

    wxSpinCtrl* m_ratingsSpinLow;
    wxSpinCtrl* m_ratingsSpinHigh;
    wxSpinCtrl* m_yearSpinLow;
    wxSpinCtrl* m_yearSpinHigh;
    wxSpinCtrl* m_epsWatchedSpinLow;
    wxSpinCtrl* m_epsWatchedSpinHigh;
    wxSpinCtrl* m_rewatchedSpinLow;
    wxSpinCtrl* m_rewatchedSpinHigh;
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
