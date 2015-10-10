#include "../Sqlite3.hpp"

cppw::Sqlite3Result::Sqlite3Result(sqlite3_stmt* statement)
	: m_statement(statement) {}

cppw::Sqlite3Result::~Sqlite3Result()
{
	sqlite3_finalize(m_statement);
}

bool cppw::Sqlite3Result::NextRow()
{
	int code;
	if((code = sqlite3_step(m_statement)) != SQLITE_ROW && code != SQLITE_DONE)
			throw Sqlite3Exception(sqlite3_db_handle(m_statement));
	return code != SQLITE_DONE;
}

int cppw::Sqlite3Result::GetInt(const int colIdx)
{
    return sqlite3_column_int(m_statement, colIdx);
}

int cppw::Sqlite3Result::GetInt(const std::string& colName)
{
    if(!m_createdMap)
        InitColMap();
    return sqlite3_column_int(m_statement, (*m_colMap)[colName]);
}

double cppw::Sqlite3Result::getDouble(const int colIdx)
{
    return sqlite3_column_double(m_statement, colIdx);
}

double cppw::Sqlite3Result::getDouble(const std::string& colName)
{
    if(!m_createdMap)
        InitColMap();
    return sqlite3_column_double(m_statement, (*m_colMap)[colName]);
}

std::string cppw::Sqlite3Result::GetString(const int colIdx)
{
    auto result = sqlite3_column_text(m_statement, colIdx);
    if(!result)
        return "";
    return reinterpret_cast<const char*>(result);
}

std::string cppw::Sqlite3Result::GetString(const std::string& colName)
{
    if(!m_createdMap)
        InitColMap();
    auto result = sqlite3_column_text(m_statement, (*m_colMap)[colName]);
    if(!result)
        return "";
    return reinterpret_cast<const char*>(result);
}

bool cppw::Sqlite3Result::GetBool(const int colIdx)
{
    if(IsNull(colIdx))
        return false;
    else if(sqlite3_column_int(m_statement, colIdx) == 0)
        return false;
    else
        return true;
}

bool cppw::Sqlite3Result::GetBool(const std::string& colName)
{
    if(!m_createdMap)
        InitColMap();
    int colIdx = (*m_colMap)[colName];
    if(IsNull(colIdx))
        return false;
    else if(sqlite3_column_int(m_statement, colIdx) == 0)
        return false;
    else
        return true;
}

bool cppw::Sqlite3Result::IsNull(const int colIdx)
{
    return sqlite3_column_type(m_statement, colIdx) == SQLITE_NULL;
}

bool cppw::Sqlite3Result::IsNull(const std::string& colName)
{
    if(!m_createdMap)
        InitColMap();
    return sqlite3_column_type(m_statement, (*m_colMap)[colName]) == SQLITE_NULL;
}

int cppw::Sqlite3Result::GetColumnCount()
{
    return sqlite3_column_count(m_statement);
}

void cppw::Sqlite3Result::InitColMap()
{
	m_colMap = std::make_unique<std::map<std::string, int>>();
	for(int i=0; i<sqlite3_column_count(m_statement); ++i){
		(*m_colMap)[sqlite3_column_name(m_statement, i)] = i;
	}
}
