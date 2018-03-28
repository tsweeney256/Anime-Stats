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

#ifndef SQLGRIDCOMMAND_HPP
#define SQLGRIDCOMMAND_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <exception>
#include <array>
#include "AppIDs.hpp"
#include "FilterStructs.hpp"

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
    virtual void SetSqlite3Connection(cppw::Sqlite3Connection* connection);

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

    using TagVal = std::array<std::string, 2>;
    using TagArray = std::vector<TagVal>;
    std::vector<std::array<std::string, selectedTitleCols>> getTitlesOfSeries(int64_t idSeries);
    void InsertIntoTitle(const std::vector<std::array<std::string, selectedTitleCols>>& titles,
                         const std::string& idSeries);
    TagArray GetTagsOfSeries(int64_t idSeries);
    void InsertIntoTag(const TagArray& tags, const std::string& idSeries);
};

//various functions and data common to both insert and update commands
class InsertableOrUpdatable
{
public:
    InsertableOrUpdatable() = delete;
    InsertableOrUpdatable(
        DataPanel* dataPanel, int label, int64_t idSeries = -1);
    virtual ~InsertableOrUpdatable() = default;

protected:
    virtual void AbstractDummy() = 0; //just to keep the class abstract
    void CheckIfLegalTitle(cppw::Sqlite3Connection* connection, const std::string& title);

    int m_idLabel;
    int64_t m_idSeries;
    DataPanel* m_dataPanel;
};

class InsertCommand : public InsertDeleteCommand, public InsertableOrUpdatable
{
public:
    InsertCommand() = delete;
    InsertCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, std::string title,
                  int idLabel); //updates the database upon construction
    void Execute() override;
    void UnExecute() override;

private:
    void AbstractDummy() override {}
    void ExecuteCommon();

    std::string m_title;
    //vector of strings instead of vector of tuples to avoid string conversions, even if it takes a bit more memory
    //numTitleCols-2 because we're not going to save idTitle and idSeries
    std::vector<std::array<std::string, selectedTitleCols>> m_titles;
    TagArray m_tags;
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
    std::vector<TagArray> m_tagsGroup;

    const std::string seriesColNames = " idSeries, rating, idReleaseType, idWatchedStatus, year, idSeason, episodesWatched, "
        "totalEpisodes, rewatchedEpisodes, episodeLength, dateStarted, dateFinished ";
};

class UpdateCommand : public SqlGridCommand, public InsertableOrUpdatable
{
public:
    UpdateCommand() = delete;
    UpdateCommand(cppw::Sqlite3Connection* connection, wxGrid* grid, DataPanel* dataPanel, int64_t idSeries,
                  std::string newVal, std::string oldVal, int wxGridCol, const std::vector<wxString>* map, int label);
    void Execute() override;
    void UnExecute() override;

private:
    void AbstractDummy() override {}
    void ExecutionCommon(const std::string& val);
    void CheckIfLegalPronunciation(const std::string& str);
    void MakeRowVisible();
    std::string GetIdName(const std::string& name);
    std::string GetIdName();

    std::string m_newVal;
    std::string m_oldVal;
    int m_col;
    const std::vector<wxString>* m_map;
};

class SqlGridCommandException : public std::exception { protected: virtual void AbstractDummy() = 0; };

class EmptyTitleException : public SqlGridCommandException
{
public:
    virtual const char* what() const noexcept override {return "Title may not be empty.";}

protected:
    void AbstractDummy() override {}
};

class DupeTitleException : public SqlGridCommandException
{
public:
    virtual const char* what() const noexcept override {return "Entry with this title already exists.";}

protected:
    void AbstractDummy() override {}
};

class BlankPronunciationException : public SqlGridCommandException
{
public:
    virtual const char* what() const noexcept override {return "Pronunciation may not consist of only blank spaces.";}
protected:
    void AbstractDummy() override {}
};

#endif
