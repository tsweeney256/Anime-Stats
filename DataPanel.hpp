#ifndef DATAPANEL_HPP
#define DATAPANEL_HPP

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <memory>
#include <vector>
#include "cppw/Sqlite3.hpp"
#include "AppIDs.hpp"
#include "SqlGridCommand.hpp"

namespace cppw { class Sqlite3Connection; }

//the panel that holds all the components that will make up the Data page
class DataPanel : public wxPanel
{
public:
    DataPanel(cppw::Sqlite3Connection* connection, wxWindow* parent, wxWindow* top, wxWindowID id = ID_NOTEBOOK,
              const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
    bool UnsavedChangesExist();
    void SetUnsavedChanges(bool);
    void Undo();
    void Redo();
    void ApplyFilter(const std::string& filterStr, bool Watched, bool Watching, bool Stalled, bool Dropped, bool Blank);

private:
    void OnGeneralWatchedStatusCheckbox(wxCommandEvent& event);
    void OnEnableAllCheckbox(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnApplyFilter(wxCommandEvent& event);
    void OnResetFilter(wxCommandEvent& event);
    void OnAddRow(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnGridColSort(wxGridEvent& event);
    void OnGridCellChanging(wxGridEvent& event);

    void ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results);
    void AppendStatusStr(std::string& statusStr, std::string toAppend, bool& firstStatus);
    void ApplyFullGrid();
    void AppendLastGridRow();
    void ApplyFilterEasy();
    void NewFilter();
    void HandleCommandChecking();

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
    std::vector<std::unique_ptr<SqlGridCommand>> m_commands;
    int m_commandLevel = 0;
    bool m_colsCreated = false;
    int m_curColSort = col::TITLE;
    bool m_curSortAsc = true;
    bool m_unsavedChanges = false;
    std::string m_oldFilterStr;
    bool m_oldWatched = true, m_oldWatching = true, m_oldStalled = true, m_oldDropped = true, m_oldBlank = true;
};

#endif // DATAPANEL_HPP
