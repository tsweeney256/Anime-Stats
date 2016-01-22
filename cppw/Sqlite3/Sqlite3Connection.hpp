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

#ifndef SQLITE3WCONNECTION_HPP_
#define SQLITE3WCONNECTION_HPP_

#include <string>
#include <memory>
#include <cstdint>
#include "../Sqlite3.hpp"

namespace cppw
{
	class Sqlite3Connection
	{
	public:
		Sqlite3Connection() = default;
		Sqlite3Connection(const std::string& filename);
		~Sqlite3Connection();
		Sqlite3Connection(const Sqlite3Connection&) = delete;
		Sqlite3Connection& operator=(const Sqlite3Connection&) = delete;
		Sqlite3Connection(Sqlite3Connection&&) = default;
		Sqlite3Connection& operator=(Sqlite3Connection&&) = default;

		void Open(const std::string& filename); //delayed construction
		void Open(const char* filename);
		void Close(); //Optional. Connection will be closed upon object destruction if this is not called.
		void Begin();
		void Commit();
		void Rollback();
		void EnableForeignKey(const bool enable);
		int64_t  GetLastInsertRowID();
		void ExecuteQuery(const std::string& query);
		std::unique_ptr<Sqlite3Statement> PrepareStatement(const std::string& statement);


	private:
		sqlite3_stmt* PrepareStatementCommon(const std::string& statement);
		sqlite3* m_connection = nullptr;
	};
}


#endif /* SQLITE3WCONNECTION_HPP_ */
