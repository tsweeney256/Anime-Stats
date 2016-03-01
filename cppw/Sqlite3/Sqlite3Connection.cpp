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

#include <cstring>
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

cppw::Sqlite3Connection cppw::Sqlite3Connection::BackupToFile(const std::string& file)
{
    Sqlite3Connection dest(file);
    auto backup = sqlite3_backup_init(dest.m_connection, "main", m_connection, "main");
    if(!backup)
        throw Sqlite3Exception(dest.m_connection);
    if(sqlite3_backup_step(backup, -1) != SQLITE_DONE)
        throw Sqlite3Exception(dest.m_connection);
    if(sqlite3_backup_finish(backup) != SQLITE_OK)
        throw Sqlite3Exception(dest.m_connection);
    return dest;
}

void cppw::Sqlite3Connection::ExecuteQuery(const std::string& query)
{
    Sqlite3Result(m_connection, query);
}

std::unique_ptr<cppw::Sqlite3Statement> cppw::Sqlite3Connection::PrepareStatement(const std::string& statementStr)
{
    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_connection,
            statementStr.c_str(),
            statementStr.size(),
            &statement,
            nullptr) != SQLITE_OK){
        throw Sqlite3Exception(m_connection);
    }
    return std::unique_ptr<Sqlite3Statement>(new Sqlite3Statement(statement));
}

std::unique_ptr<cppw::Sqlite3Statement> cppw::Sqlite3Connection::PrepareStatement(const char* statementStr, size_t size)
{
    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(m_connection,
            statementStr,
            size,
            &statement,
            nullptr) != SQLITE_OK){
        throw Sqlite3Exception(m_connection);
    }
    return std::unique_ptr<Sqlite3Statement>(new Sqlite3Statement(statement));
}

int64_t cppw::Sqlite3Connection::GetLastInsertRowID()
{
    return sqlite3_last_insert_rowid(m_connection);
}

void cppw::Sqlite3Connection::SetLogging(std::ostream* os)
{
    sqlite3_trace(m_connection, callbackFunction, static_cast<void*>(os));
}

cppw::Sqlite3Connection::Sqlite3Connection(Sqlite3Connection&& other)
{
    m_connection = other.m_connection;
    other.m_connection = nullptr;
}

cppw::Sqlite3Connection& cppw::Sqlite3Connection::operator=(Sqlite3Connection&& rhs)
{
    m_connection = rhs.m_connection;
    rhs.m_connection = nullptr;
    return *this;
}

void cppw::Sqlite3Connection::callbackFunction(void* data, const char* output)
{
    *static_cast<std::ostream*>(data) << output << std::endl;
}
