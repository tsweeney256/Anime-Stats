#ifndef SQLITE3WCONNECTION_HPP_
#define SQLITE3WCONNECTION_HPP_

#include <string>
#include <memory>
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
		void ExecuteQuery(const std::string& query);
		std::unique_ptr<Sqlite3Statement> PrepareStatement(const std::string& statement);


	private:
		sqlite3_stmt* PrepareStatementCommon(const std::string& statement);
		sqlite3* m_connection = nullptr;
	};
}


#endif /* SQLITE3WCONNECTION_HPP_ */
