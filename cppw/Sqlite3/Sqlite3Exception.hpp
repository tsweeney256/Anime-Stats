#ifndef CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_
#define CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_

#include <string>
#include "../Sqlite3.hpp"

namespace cppw
{
	class Sqlite3Exception
	{
	public:
		Sqlite3Exception(sqlite3* connection);

		int GetErrorCode() const;
		int GetExtendedErrorCode() const;
		std::string GetErrorMessage() const;

	private:
		sqlite3* m_connection;
	};
}



#endif /* CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_ */
