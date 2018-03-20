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

#ifndef CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_
#define CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_

#include <string>
#include <exception>
#include "../Sqlite3.hpp"

namespace cppw
{
	class Sqlite3Exception : public std::exception
	{
	public:
		Sqlite3Exception(sqlite3* connection, int errCode);

		int GetErrorCode() const;
		virtual const char* what() const noexcept;

	private:
        sqlite3* m_connection;
		int m_errCode;
	};
}



#endif /* CPPW_SQLITE3_SQLITE3EXCEPTION_HPP_ */
