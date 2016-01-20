#ifndef SQLGRIDCOMMAND_HPP
#define SQLGRIDCOMMAND_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <exception>
#include "AppIDs.hpp"

namespace cppw { class Sqlite3Connection;
                 class Sqlite3Statement; }
class wxGrid;
class wxString;
class DataPanel;

class SqlGridCommand //abstract
{
public:
    SqlGridCommand(cppw::Sqlite3Connection* connection, wxGrid* grid);
    virtual ~SqlGridCommand() = default;
    virtual void Execute() = 0;
    virtual void UnExecute() = 0;

protected:
    int GetRowWithIdSeries(int64_t idSeries);

    cppw::Sqlite3Connection* m_connection;
    wxGrid* m_grid;
};

class InsertDeleteCommand : public SqlGridCommand
{
public:
    InsertDeleteCommand(cppw::Sqlite3Connection* connection, wxGrid* grid);

protected:
    static const int selectedTitleCols = numTitleCols-2;

    std::vector<std::array<std::string, selectedTitleCols>> getTitlesOfSeries(int64_t idSeries);
    void InsertIntoTitle(const std::vector<std::array<std::string, selectedTitleCols>>& titles,
            const std::string& idSeries);

    //sqlite3 doesn't support multithreading anyway, so there's no downsides to static
    static std::unique_ptr<cppw::Sqlite3Statement> m_titleSelectStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_titleInsertStmt;
};

//various functions and data common to both insert and update commands
class InsertableOrUpdatable
{
public:
    InsertableOrUpdatable() = delete;
    InsertableOrUpdatable(cppw::Sqlite3Connection* connection, DataPanel* dataPanel,
            std::shared_ptr<std::vector<wxString>> addedRowIDs, int label, int64_t idSeries = -1);
    virtual ~InsertableOrUpdatable() = default;

protected:
    virtual void AbstractDummy() = 0; //just to keep the class abstract
    void AddRowIDToFilterList();
    void RemoveRowIDFromFilterList();
    void CheckIfLegalTitle(std::string title);

    int m_idLabel;
    int64_t m_idSeries;
    DataPanel* m_dataPanel;
    std::shared_ptr<std::vector<wxString>> m_addedRowIDs;

    static std::unique_ptr<cppw::Sqlite3Statement> m_dupeTitleCheckStmt;
};

class InsertCommand : public InsertDeleteCommand, public InsertableOrUpdatable
{
public:
    InsertCommand() = delete;
    InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, std::string title,
            int idLabel, std::shared_ptr<std::vector<wxString>> addedRowIDs); //updates the database upon construction
    void Execute() override;
    void UnExecute() override;

private:
    void AbstractDummy() override {}
    void ExecuteCommon();

    std::string m_title;
    //vector of strings instead of vector of tuples to avoid string conversions, even if it takes a bit more memory
    //numTitleCols-2 because we're not going to save idTitle and idSeries
    std::vector<std::array<std::string, selectedTitleCols>> m_titles;
    static std::unique_ptr<cppw::Sqlite3Statement> m_deleteRowStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_insertBlankStmt;
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
    static std::unique_ptr<cppw::Sqlite3Statement> m_seriesDeleteStmt;
    const std::string seriesColNames = " idSeries, rating, idReleaseType, idWatchedStatus, year, idSeason, episodesWatched, "
            "totalEpisodes, rewatchedEpisodes, episodeLength, dateStarted, dateFinished ";
};

class UpdateCommand : public SqlGridCommand, public InsertableOrUpdatable
{
public:
    UpdateCommand() = delete;
    UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, int64_t idSeries,
            std::string newVal, std::string oldVal, int wxGridCol, const std::vector<wxString>* map, int label,
            std::shared_ptr<std::vector<wxString>> addedRowIDs);
    void Execute() override;
    void UnExecute() override;

private:
    void AbstractDummy() override {}
    void ExecutionCommon(const std::string& newVal, const std::string& oldVal);

    std::string m_newVal;
    std::string m_oldVal;
    int m_col;
    const std::vector<wxString>* m_map;

    static std::unique_ptr<cppw::Sqlite3Statement> m_selectIdTitleStmt;
    static std::unique_ptr<cppw::Sqlite3Statement> m_updateTitleStmt;
};

class FilterCommand : public SqlGridCommand
{
public:
    FilterCommand() = delete;
    FilterCommand(DataPanel* dataPanel, std::string newFilterStr, std::string oldFilterStr, bool newWatched, bool newWatching,
            bool newStalled, bool newDropped, bool newBlank, bool oldWatched, bool oldWatching, bool oldStalled, bool oldDropped,
            bool oldBlank, std::shared_ptr<std::vector<wxString>> addedRowIDs); //this is obnoxious
    void Execute() override;
    void UnExecute() override;

private:
    DataPanel* m_dataPanel;
    std::string m_newFilterStr;
    std::string m_oldFilterStr;
    bool m_newWatched, m_newWatching, m_newStalled, m_newDropped, m_newBlank;
    bool m_oldWatched, m_oldWatching, m_oldStalled, m_oldDropped, m_oldBlank;
    std::shared_ptr<std::vector<wxString>> m_addedRowIDs;
};

class SqlGridCommandException : public std::exception { protected: virtual void AbstractDummy() = 0; };

class EmptyTitleException : public SqlGridCommandException
{
public:
    virtual const char* what() const noexcept override {return "Title may not be empty.";}

protected:
    void AbstractDummy() {}
};

class DupeTitleException : public SqlGridCommandException
{
public:
    virtual const char* what() const noexcept override {return "Entry with this title already exists.";}

protected:
    void AbstractDummy() {}
};

#endif
