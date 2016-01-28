#ifndef TITLEALIASDIALOG_HPP
#define TITLEALIASDIALOG_HPP

#include <cstdint>
#include <wx/dialog.h>

namespace cppw{ class Sqlite3Connection; }


class TitleAliasDialog : public wxDialog
{
public:
    TitleAliasDialog() = delete;
    TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries);

private:
    cppw::Sqlite3Connection* m_connection;
    int64_t m_idSeries;
};

#endif
