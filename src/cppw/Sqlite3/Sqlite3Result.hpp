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

#ifndef CPPW_SQLITE3_Sqlite3Result_HPP_
#define CPPW_SQLITE3_Sqlite3Result_HPP_

#include <map>
#include "../Sqlite3.hpp"

namespace cppw
{
	class Sqlite3Result
	{
	public:
		~Sqlite3Result() = default;
		Sqlite3Result() = delete;
		Sqlite3Result(const Sqlite3Result&) = delete;
		Sqlite3Result& operator=(const Sqlite3Result&) = delete;
		Sqlite3Result(Sqlite3Result&&) = default;
		Sqlite3Result& operator=(Sqlite3Result&&) = default;

		bool NextRow();
		int GetInt(const int colIdx);
		int GetInt(const std::string& colName);
		double getDouble(const int colIdx);
		double getDouble(const std::string& colName);
		std::string GetString(const int colIdx);
		std::string GetString(const std::string& colName);
		bool GetBool(const int colIdx);
		bool GetBool(const std::string& colName);
		bool IsNull(const int colIdx);
		bool IsNull(const std::string& colName);
		std::string GetColumnName(const int colIdx);
		int GetColumnCount();
		void Reset(bool throws=false); //same thing as Sqlite3Statement::Reset()

	private:
		friend class Sqlite3Connection;
		friend class Sqlite3Statement;
		Sqlite3Result(sqlite3_stmt* statement);
		Sqlite3Result(sqlite3* connection, std::string query);
		void InitColMap();
		sqlite3_stmt* m_statement;
		//pointer so that users who don't use the string search overloads don't have to suffer a needless construction cost
		std::unique_ptr<std::map<std::string, int>> m_colMap;
		bool m_createdMap = false;
	};
}



#endif /* CPPW_SQLITE3_Sqlite3Result_HPP_ */
