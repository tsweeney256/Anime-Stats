#include "../Sqlite3.hpp"

cppw::Sqlite3Exception::Sqlite3Exception(sqlite3* connection)
    : m_connection(connection) {}

int cppw::Sqlite3Exception::GetErrorCode() const
{
    return sqlite3_errcode(m_connection);
}

int cppw::Sqlite3Exception::GetExtendedErrorCode() const
{
    return sqlite3_extended_errcode(m_connection);
}

std::string cppw::Sqlite3Exception::GetErrorMessage() const
{
    return sqlite3_errmsg(m_connection);
}
