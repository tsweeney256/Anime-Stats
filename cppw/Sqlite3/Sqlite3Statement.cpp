#include <cstring>
#include "../Sqlite3.hpp"


cppw::Sqlite3Statement::Sqlite3Statement(sqlite3_stmt* statement)
    : m_statement(statement) {}

cppw::Sqlite3Statement::~Sqlite3Statement()
{
    sqlite3_finalize(m_statement);
}

void cppw::Sqlite3Statement::Bind(const int paramIdx, const int val)
{
    if(sqlite3_bind_int(m_statement, paramIdx, val) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::Bind(const int paramIdx, const double val)
{
    if(sqlite3_bind_double(m_statement, paramIdx, val) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::Bind(const int paramIdx, const std::string& val)
{
    if(sqlite3_bind_text(m_statement, paramIdx, val.c_str(), val.size(), nullptr) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

//solely because string literals like to use the bool version for some reason...
void cppw::Sqlite3Statement::Bind(const int paramIdx, const char* val)
{
    if(sqlite3_bind_text(m_statement, paramIdx, val, strlen(val), nullptr) != SQLITE_OK)
            throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::Bind(const int paramIdx, const bool val)
{
    if(sqlite3_bind_int(m_statement, paramIdx, val) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::BindNull(const int paramIdx)
{
    if(sqlite3_bind_null(m_statement, paramIdx) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::ClearBindings()
{
    if(sqlite3_clear_bindings(m_statement) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::Reset()
{
    if(sqlite3_reset(m_statement) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3_db_handle(m_statement));
}

void cppw::Sqlite3Statement::Finalize()
{
    sqlite3_finalize(m_statement);
    m_statement = nullptr;
}

int cppw::Sqlite3Statement::GetParamCount()
{
    return sqlite3_bind_parameter_count(m_statement);
}

std::unique_ptr<cppw::Sqlite3Result> cppw::Sqlite3Statement::GetResults()
{
    return std::unique_ptr<cppw::Sqlite3Result>(new cppw::Sqlite3Result(m_statement));
}

/*cppw::Sqlite3Statement::Sqlite3Statement(cppw::Sqlite3Statement&& other)
{
    other.m_finalized = m_finalized;
    other.m_statement = m_statement;
    m_statement = nullptr;
}

cppw::Sqlite3Statement&& cppw::Sqlite3Statement::operator =(Sqlite3Statement&& rhs)
{
    if(this != &rhs){
        rhs.m_finalized = m_finalized;
        rhs.m_statement = m_statement;
        m_statement = nullptr;
    }
}*/
