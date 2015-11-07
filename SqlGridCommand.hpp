#ifndef SQLGRIDCOMMAND_HPP
#define SQLGRIDCOMMAND_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include "AppIDs.hpp"

namespace cppw { class Sqlite3Connection;
                 class Sqlite3Statement; }
class wxGrid;

class SqlGridCommand //abstract
{
public:
    SqlGridCommand(cppw::Sqlite3Connection* connection, wxGrid* grid);
    virtual ~SqlGridCommand() = default;
    virtual void Execute() = 0;
    virtual void UnExecute() = 0;

protected:
    //does things like replace ' with '' and replace the empty string with null
    void FormatString(std::string& str);
    int GetRowWithIdSeries(int64_t idSeries);

    cppw::Sqlite3Connection* m_connection;
    wxGrid* m_grid;
};

class InsertDeleteCommand : public SqlGridCommand
{
public:
    InsertDeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid);
    void Execute() override = 0;
    void UnExecute() override = 0;

protected:
    static const int selectedTitleCols = numTitleCols-2;

    std::vector<std::array<std::string, selectedTitleCols>> getTitlesOfSeries(int64_t idSeries);
    void InsertIntoTitle(const std::vector<std::array<std::string, selectedTitleCols>>& titles,
            const std::string& idSeries);

    //sqlite3 doesn't support multithreading anyway, so there's no downsides to static
    static std::unique_ptr<cppw::Sqlite3Statement> m_titleSelectStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_titleInsertStmt;
};

class InsertCommand : public InsertDeleteCommand
{
public:
    InsertCommand() = delete;
    InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, std::string title,
            int idLabel); //updates the database upon construction
    void Execute() override;
    void UnExecute() override;

private:
    void ExecuteCommon();

    int64_t m_idSeries;
    std::string m_title;
    int m_idLabel;
    //vector of strings instead of vector of tuples to avoid string conversions, even if it takes a bit more memory
    //numTitleCols-2 because we're not going to save idTitle and idSeries
    std::vector<std::array<std::string, selectedTitleCols>> m_titles;
};

class DeleteCommand : public InsertDeleteCommand
{
public:
    DeleteCommand() = delete;
    //updates the database upon construction
    DeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, std::vector<int64_t> idSeries);
    void Execute() override;
    void UnExecute() override;

private:
    void ExecuteCommon();

    std::vector<int64_t> m_idSeries;
    std::vector<std::array<std::string, numSeriesCols>> m_series;
    std::vector<std::array<std::string, numViewCols>> m_seriesView;
    std::vector<std::vector<std::array<std::string, selectedTitleCols>>> m_titlesGroup; //multiple entries will each have multiple names

    static std::unique_ptr<cppw::Sqlite3Statement> m_seriesSelectStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_seriesInsertStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_seriesViewSelectStmt;
    const std::string seriesColNames = " idSeries, rating, idReleaseType, idWatchedStatus, year, idSeason, episodesWatched, "
            "totalEpisodes, rewatchedEpisodes, episodeLength, dateStarted, dateFinished ";
};

class UpdateCommand : public SqlGridCommand
{
public:
    UpdateCommand() = delete;
    UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, int64_t idSeries,
            std::string newVal, std::string oldVal, int wxGridCol);
    void Execute() override;
    void UnExecute() override;

private:
    void ExecutionCommon(const std::string& newVal, const std::string& oldVal);
    std::string FormatUpdate(const std::string& val);

    int64_t m_idSeries;
    std::string m_newGridVal;
    std::string m_oldGridVal;
    std::string m_newDbVal;
    std::string m_oldDbVal;
    int m_col;

    static std::unique_ptr<cppw::Sqlite3Statement> m_selectIdTitleStmt;
};

#endif
