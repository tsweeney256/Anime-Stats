/*Anime Stats
  Copyright (C) 2015-2018 Thomas Sweeney
  This file is part of Anime Stats.
  Anime Stats is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Anime Stats is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <wx/listctrl.h>
#include "SqlTitleAliasCommand.hpp"
#include "cppw/Sqlite3.hpp"

SqlTitleAliasCommand::SqlTitleAliasCommand::SqlTitleAliasCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, int listPos)
    : m_connection(connection), m_list(list), m_idSeries(idSeries), m_listPos(listPos) {}

SqlTitleAliasCommand::UpdateCommand::UpdateCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries,
                                                   int listPos, std::string newVal, std::string oldVal)
    : SqlTitleAliasCommand(connection, list, idSeries, listPos), m_newVal(newVal), m_oldVal(oldVal)
{
    m_updateStmt = m_connection->PrepareStatement("update Title set name=? where name=? and idSeries=?");
}

void SqlTitleAliasCommand::UpdateCommand::Execute()
{
    ExecuteCommon(m_newVal);
}

void SqlTitleAliasCommand::UpdateCommand::UnExecute()
{
    ExecuteCommon(m_oldVal);
}

void SqlTitleAliasCommand::UpdateCommand::ExecuteCommon(std::string newVal)
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
    m_insertStmt = m_connection->PrepareStatement("insert into Title (name, idSeries, idLabel) values(?,?,0)");
}

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
    m_deleteStmt = m_connection->PrepareStatement("delete from Title where name=? and idSeries=?");
}

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
