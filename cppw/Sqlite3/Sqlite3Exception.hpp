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
