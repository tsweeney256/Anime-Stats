#ifndef DATAPANEL_HPP
#define DATAPANEL_HPP

#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/string.h>
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
    void ApplyFilter(const std::string& filterStr, bool Watched, bool Watching, bool Stalled,
            bool Dropped, bool Blank, std::vector<wxString>* changedRows = nullptr);

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
    void OnComboDropDown(wxCommandEvent& event);

    void ResetTable(std::unique_ptr<cppw::Sqlite3Result>& results);
    void AppendStatusStr(std::string& statusStr, std::string toAppend, bool& firstStatus);
    void ApplyFullGrid();
    void AppendLastGridRow(bool whiteOutPrevious);
    void ApplyFilterEasy();
    void NewFilter();
    void HandleCommandChecking();
    void BuildAllowedValsMap(std::vector<wxString>& map, const std::string& sqlStmtStr);
    void SetRatingColor(int row, const char* valStr);
    void SetWatchedStatusColor(int row, const std::string& valStr);
    void HandleUndoRedoColorChange();
    void UpdateOldFilterData();
    std::string GetAddedRowsSqlStr(std::vector<wxString>* changedRows);

    class ratingColor{
    public:
        enum{MIN, MID, MAX};
        enum{R, G, B};
    };

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
    std::string m_oldCellComboIndex;
    std::vector<std::unique_ptr<SqlGridCommand>> m_commands;
    std::vector<wxString> m_allowedWatchedVals;
    std::vector<wxString> m_allowedReleaseVals;
    std::vector<wxString> m_allowedSeasonVals;
    int m_commandLevel = 0;
    bool m_colsCreated = false;
    int m_curColSort = col::TITLE;
    bool m_curSortAsc = true;
    bool m_unsavedChanges = false;
    std::string m_oldFilterStr;
    bool m_oldWatched = true, m_oldWatching = true, m_oldStalled = true, m_oldDropped = true, m_oldBlank = true;
    bool m_ratingColorEnabled = true, m_watchedStatusColorEnabled = true;
    int m_ratingColor[3][3] {{248, 105, 107}, {255, 235, 132}, {99, 190, 123}};
    //the hex is bgr instead of rgb because wxwidgets implemented it backwards
    std::vector<wxColour> m_watchedStatusColor {0xFFFFFF, 0xFFC271, 0x94FF3B, 0x83B8F9, 0x7D7DFF};
    int m_minRating = 0;
    int m_midRating = 5;
    int m_maxRating = 9;
    wxBoxSizer* m_panelSizer;
    std::unique_ptr<std::vector<wxString>> m_changedRows;

    DECLARE_EVENT_TABLE()
};

#endif // DATAPANEL_HPP
