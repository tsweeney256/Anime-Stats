#ifndef TITLEALIASDIALOG_HPP
#define TITLEALIASDIALOG_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <wx/dialog.h>

namespace cppw { class Sqlite3Connection; }
class wxListCtrl;
class wxListEvent;
namespace SqlTitleAliasCommand { class SqlTitleAliasCommand; }


class TitleAliasDialog : public wxDialog
{
public:
    TitleAliasDialog() = delete;
    TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries, wxString title);

private:
    void OnSelect(wxListEvent& event);
    void OnBeginUpdate(wxListEvent& event);
    void OnEndUpdate(wxListEvent& event);
    void OnDelete(wxListEvent& event);
    void OnOk(wxCommandEvent& event);

    cppw::Sqlite3Connection* m_connection;
    int64_t m_idSeries;
    wxString m_title;
    wxListCtrl* m_list = nullptr;
    std::vector<std::unique_ptr<SqlTitleAliasCommand::SqlTitleAliasCommand>> m_commands;
    wxString m_oldDelVal;
    wxString m_oldEditVal;

    DECLARE_EVENT_TABLE()
};

#endif
