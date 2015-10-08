#ifndef DATAPANEL_HPP
#define DATAPANEL_HPP

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include "AppIDs.hpp"

//the panel that holds all the components that will make up the Data page
class DataPanel : public wxPanel
{
public:
    DataPanel(wxWindow* parent, wxWindowID id = ID_NOTEBOOK,
              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
private:
    void OnGeneralWatchedStatusCheckbox(wxCommandEvent& event);
    void OnEnableAllCheckbox(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    void OnResetFilter(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);

    //DECLARE_EVENT_TABLE();

    //DataGrid* m_grid;

    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_blankCheck;
    wxCheckBox* m_allCheck;

    wxTextCtrl* m_titleFilterTextField;
};

#endif // DATAPANEL_HPP
