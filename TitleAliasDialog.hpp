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

//Originally I wanted to put a wxMenu on here to allow undoing and redoing actions. The code supporting undoing
//and redoing is even already there in SqlTitleAliasCommand. However, wxDialog does not support
//having menus. This makes it so wxDialogs can not support undoing and redoing without having to tinker with how
//wxWidgets handles keyboard events. The choice became having a wxFrame that supports menus but does not support
//being modal, or having a wxDialog that does not support menus but does support being modal. I chose making the window modal.

class TitleAliasDialog : public wxDialog
{
public:
    TitleAliasDialog() = delete;
    TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries, wxString title);

private:
    void OnSelect(wxListEvent& event);
    void OnActivated(wxListEvent& event);
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
    bool m_ignoreDeleteEvent = false;;

    DECLARE_EVENT_TABLE()
};

#endif
