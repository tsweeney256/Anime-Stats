#ifndef SQLTITLEALIASCOMMAND_HPP
#define SQLTITLEALIASCOMMAND_HPP

#include <string>
#include <cstdint>
#include <memory>

namespace cppw{
    class Sqlite3Connection;
    class Sqlite3Statement;
}
class wxListCtrl;

namespace SqlTitleAliasCommand
{
    class SqlTitleAliasCommand
    {
    public:
        SqlTitleAliasCommand() = delete;
        SqlTitleAliasCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, int listPos);
        virtual ~SqlTitleAliasCommand() = default;

        virtual void Execute() = 0;
        virtual void UnExecute() = 0;
        virtual void ApplyToDb() = 0;

    protected:
        cppw::Sqlite3Connection* m_connection;
        wxListCtrl* m_list;
        int64_t m_idSeries;
        int m_listPos;
    };

    class UpdateCommand : public SqlTitleAliasCommand
    {
    public:
        UpdateCommand() = delete;
        UpdateCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries,
                int listPos, std::string newVal, std::string oldVal);

        void Execute() override;
        void UnExecute() override;
        void ApplyToDb() override;

    private:
        void ExecuteCommon(std::string targetVal, std::string newVal);

        std::string m_newVal;
        std::string m_oldVal;

        static std::unique_ptr<cppw::Sqlite3Statement> m_updateStmt;
    };

    class InsertCommand : public SqlTitleAliasCommand
    {
    public:
        InsertCommand() = delete;
        InsertCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, std::string val);

        void Execute() override;
        void UnExecute() override;
        void ApplyToDb() override;

    private:
        std::string m_val;

        static std::unique_ptr<cppw::Sqlite3Statement> m_insertStmt;
    };

    class DeleteCommand : public SqlTitleAliasCommand
    {
    public:
        DeleteCommand() = delete;
        DeleteCommand(cppw::Sqlite3Connection* connection, wxListCtrl* list, int64_t idSeries, int listPos, std::string val);

        void Execute() override;
        void UnExecute() override;
        void ApplyToDb() override;

    private:
        std::string m_val;

        static std::unique_ptr<cppw::Sqlite3Statement> m_deleteStmt;
    };
}

#endif
