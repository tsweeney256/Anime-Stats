/*Anime Stats
  Copyright (C) 2015-2018 Thomas Sweeney
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
#include <ostream>
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
		Sqlite3Connection(Sqlite3Connection&&);
		Sqlite3Connection& operator=(Sqlite3Connection&&);

		void Open(const std::string& filename); //delayed construction
		void Open(const char* filename);
		void Close(); //Optional. Connection will be closed upon object destruction if this is not called.
		void Begin();
		void Commit();
		void Rollback();
		void EnableForeignKey(const bool enable);
		int64_t GetLastInsertRowID();
		void ExecuteQuery(const std::string& query);
		void SetLogging(std::ostream* os);
		Sqlite3Connection BackupToFile(const std::string& file);
		std::unique_ptr<Sqlite3Statement> PrepareStatement(const std::string& statementStr);
		std::unique_ptr<Sqlite3Statement> PrepareStatement(const char* statementStr, size_t size = -1);


	private:
		static void callbackFunction(void* data, const char* output);
		sqlite3* m_connection = nullptr;
	};
}


#endif /* SQLITE3WCONNECTION_HPP_ */
