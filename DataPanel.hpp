#ifndef DATAPANEL_HPP
#define DATAPANEL_HPP

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <memory>
#include "cppw/Sqlite3.hpp"
#include "AppIDs.hpp"

namespace cppw { class Sqlite3Connection; }

//the panel that holds all the components that will make up the Data page
class DataPanel : public wxPanel
{
public:
    DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, wxWindow* top, wxWindowID id = ID_NOTEBOOK,
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
    void OnGridColSort(wxGridEvent& event);

    void ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results);
    void ApplyFilter();
    void AppendStatusStr(std::string& statusStr, std::string toAppend, bool& firstStatus);
    void ApplyFullGrid();

    DECLARE_EVENT_TABLE();

    wxWindow* m_top;
    wxGrid* m_grid;
    wxCheckBox* m_watchedCheck;
    wxCheckBox* m_watchingCheck;
    wxCheckBox* m_stalledCheck;
    wxCheckBox* m_droppedCheck;
    wxCheckBox* m_blankCheck;
    wxCheckBox* m_allCheck;
    wxTextCtrl* m_titleFilterTextField;
    wxString m_basicSelectString;
    cppw::Sqlite3Connection* m_connection;
    std::string m_curOrderCol = "Title collate nocase";
    std::string m_curOrderDir = "asc";
    bool m_colsCreated = false;
    int m_curColSort = 2; //title
    bool m_curSortAsc = true;
    static const int m_numCols = 14; //including the hidden 3
};

#endif // DATAPANEL_HPP
