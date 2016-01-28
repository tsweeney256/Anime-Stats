#include "cppw/Sqlite3.hpp"
#include "TitleAliasDialog.hpp"

TitleAliasDialog::TitleAliasDialog(wxWindow* parent, wxWindowID id, cppw::Sqlite3Connection* connection, int64_t idSeries)
    : wxDialog(parent, id, "Title Alias"), m_connection(connection), m_idSeries(idSeries)
{

}
