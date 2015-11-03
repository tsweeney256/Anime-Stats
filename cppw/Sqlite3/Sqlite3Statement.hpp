#ifndef SQLITE3STATEMENT_HPP_
#define SQLITE3STATEMENT_HPP_

#include "../Sqlite3.hpp"

namespace cppw
{
	class Sqlite3Statement
	{
	public:
		~Sqlite3Statement();
		Sqlite3Statement() = delete;
		Sqlite3Statement(const Sqlite3Statement&) = delete;
		Sqlite3Statement& operator=(const Sqlite3Statement&) = delete;
		Sqlite3Statement(Sqlite3Statement&&) = default;
		Sqlite3Statement& operator=(Sqlite3Statement&&) = default;

		void Bind(const int paramIdx, const int val);
		void Bind(const int paramIdx, const double val);
		void Bind(const int paramIdx, const std::string& val);
		void Bind(const int paramIdx, const char* val);
		void Bind(const int paramIdx, const bool val);
		void BindNull(const int paramIdx);
		void ClearBindings();
		void Reset();
		void Finalize(); //Optional. Throws. Will be done upon object destruction if it has not been done already.
		int GetParamCount();
		std::unique_ptr<Sqlite3Result> GetResults();


	private:
		friend class Sqlite3Connection;
		Sqlite3Statement(sqlite3_stmt* statement);
		sqlite3_stmt* m_statement;
		bool m_finalized = false;
	};
}



#endif /* SQLITE3STATEMENT_HPP_ */
