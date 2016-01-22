/*Anime Stats
Copyright (C) 2016 Thomas Sweeney
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

#include "../Sqlite3.hpp"

cppw::Sqlite3Connection::Sqlite3Connection(const std::string& filename)
{
	Open(filename);
}

cppw::Sqlite3Connection::~Sqlite3Connection()
{
	sqlite3_close(m_connection);
}

void cppw::Sqlite3Connection::Open(const std::string& filename)
{
	if(sqlite3_open(filename.c_str(), &m_connection) != SQLITE_OK){
		throw Sqlite3Exception(m_connection);
	}
}

void cppw::Sqlite3Connection::Open(const char* filename)
{
    if(sqlite3_open(filename, &m_connection) != SQLITE_OK){
        throw Sqlite3Exception(m_connection);
    }
}

void cppw::Sqlite3Connection::Close()
{
	sqlite3_close(m_connection);
	m_connection = nullptr;
}

void cppw::Sqlite3Connection::Begin()
{
	ExecuteQuery("BEGIN TRANSACTION");
}

void cppw::Sqlite3Connection::Commit()
{
	ExecuteQuery("COMMIT");
}

void cppw::Sqlite3Connection::Rollback()
{
	ExecuteQuery("ROLLBACK");
}

void cppw::Sqlite3Connection::EnableForeignKey(const bool enable)
{
	std::string fk = "PRAGMA foreign_keys = ";
	if(enable) fk += "ON";
	else fk += "OFF";
	ExecuteQuery(fk);
}

sqlite3_stmt* cppw::Sqlite3Connection::PrepareStatementCommon(const std::string& statementStr)
{
	sqlite3_stmt* statement;
	if(sqlite3_prepare_v2(m_connection,
			statementStr.c_str(),
			statementStr.size(),
			&statement,
			nullptr) != SQLITE_OK){
		throw Sqlite3Exception(m_connection);
	}
	return statement;
}

void cppw::Sqlite3Connection::ExecuteQuery(const std::string& query)
{
    Sqlite3Result(m_connection, query);
}

std::unique_ptr<cppw::Sqlite3Statement> cppw::Sqlite3Connection::PrepareStatement(const std::string& statement)
{
    return std::unique_ptr<Sqlite3Statement>(new Sqlite3Statement(PrepareStatementCommon(statement)));
}

int64_t cppw::Sqlite3Connection::GetLastInsertRowID()
{
    return sqlite3_last_insert_rowid(m_connection);
}
