#include <wx/listctrl.h>
#include "SqlTitleAliasCommand.hpp"
#include "cppw/Sqlite3.hpp"

SqlTitleAliasCommand::SqlTitleAliasCommand::SqlTitleAliasCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, int listPos)
    : m_connection(connection), m_list(list), m_idSeries(idSeries), m_listPos(listPos) {}

SqlTitleAliasCommand::UpdateCommand::UpdateCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries,
        int listPos, std::string newVal, std::string oldVal)
    : SqlTitleAliasCommand(connection, list, idSeries, listPos), m_newVal(newVal), m_oldVal(oldVal)
{
    if(!m_updateStmt)
        m_updateStmt = m_connection->PrepareStatement("update Title set name=? where name=? and idSeries=?");
}

std::unique_ptr<cppw::Sqlite3Statement> SqlTitleAliasCommand::UpdateCommand::m_updateStmt(nullptr);

void SqlTitleAliasCommand::UpdateCommand::Execute()
{
    ExecuteCommon(m_oldVal, m_newVal);
}

void SqlTitleAliasCommand::UpdateCommand::UnExecute()
{
    ExecuteCommon(m_newVal, m_oldVal);
}

void SqlTitleAliasCommand::UpdateCommand::ExecuteCommon(std::string targetVal, std::string newVal)
{
    m_list->SetItem(m_listPos, 1, newVal);
}

void SqlTitleAliasCommand::UpdateCommand::ApplyToDb()
{
    m_updateStmt->Reset();
    m_updateStmt->ClearBindings();
    m_updateStmt->Bind(1, m_newVal);
    m_updateStmt->Bind(2, m_oldVal);
    m_updateStmt->Bind(3, m_idSeries);
    auto results = m_updateStmt->GetResults();
    results->NextRow();
}

SqlTitleAliasCommand::InsertCommand::InsertCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, std::string val)
    : SqlTitleAliasCommand(connection, list, idSeries, 0), m_val(val)
{
    if(!m_insertStmt)
        m_insertStmt = m_connection->PrepareStatement("insert into Title (name, idSeries, idLabel) values(?,?,0)");
}

std::unique_ptr<cppw::Sqlite3Statement> SqlTitleAliasCommand::InsertCommand::m_insertStmt(nullptr);

void SqlTitleAliasCommand::InsertCommand::Execute()
{
    m_list->InsertItem(m_listPos, m_val);
}

void SqlTitleAliasCommand::InsertCommand::UnExecute()
{
    m_list->DeleteItem(m_listPos);
}

void SqlTitleAliasCommand::InsertCommand::ApplyToDb()
{
    m_insertStmt->Reset();
    m_insertStmt->ClearBindings();
    m_insertStmt->Bind(1, m_val);
    m_insertStmt->Bind(2, m_idSeries);
    auto results = m_insertStmt->GetResults();
    results->NextRow();
}

SqlTitleAliasCommand::DeleteCommand::DeleteCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries,
        int listPos, std::string val)
    : SqlTitleAliasCommand(connection, list, idSeries, listPos), m_val(val)
{
    if(!m_deleteStmt)
        m_deleteStmt = m_connection->PrepareStatement("delete from Title where name=? and idSeries=?");
}

std::unique_ptr<cppw::Sqlite3Statement> SqlTitleAliasCommand::DeleteCommand::m_deleteStmt(nullptr);

void SqlTitleAliasCommand::DeleteCommand::Execute()
{
    m_list->DeleteItem(m_listPos);
}

void SqlTitleAliasCommand::DeleteCommand::UnExecute()
{

    m_list->InsertItem(m_listPos, m_val);
}

void SqlTitleAliasCommand::DeleteCommand::ApplyToDb()
{
    m_deleteStmt->Reset();
    m_deleteStmt->ClearBindings();
    m_deleteStmt->Bind(1, m_val);
    m_deleteStmt->Bind(2, m_idSeries);
    auto results = m_deleteStmt->GetResults();
    results->NextRow();
}
