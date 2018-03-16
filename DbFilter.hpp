/*Anime Stats
  Copyright (C) 2018 Thomas Sweeney
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

#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "FilterStructs.hpp"
#include "SortStruct.hpp"

namespace cppw
{
    class Sqlite3Connection;
}

class DbFilter
{
public:
    DbFilter() = default;
    explicit DbFilter(cppw::Sqlite3Connection* connection);

    void Reset(cppw::Sqlite3Connection* connection);
    void LoadFilter(std::string name);
    void SaveFilter(std::string name, bool makeDefault);
    void DeleteFilter(std::string name);
    void SetDefaultFilter(std::string name);
    std::vector<std::string> GetFilterNames();
    std::string GetDefaultFilterName();
    Filter GetFilter();
    ConstFilter GetFilter() const;
    std::vector<colSort>* GetSort();
    const std::vector<colSort>* GetSort() const;
    void SetFilter(std::unique_ptr<BasicFilterInfo> bfi,
                   std::unique_ptr<AdvFilterInfo> afi);
    void SetSort(std::vector<colSort> sortingRules);


private:
    int GetFilterFromDb(std::string name);
    void InsertFilterArg(int idSavedFilterArgName, int value);
    int InsertFilterToDb(std::string name, bool makeDefault);
    void SaveSortEntryToDb(colSort sortingRule);
    void SaveSortToDb(int idSavedFilter);
    void LoadSortFromDb(int idSavedFilter);

    std::unique_ptr<AdvFilterInfo> m_advFilterInfo;
    std::unique_ptr<BasicFilterInfo> m_basicFilterInfo;
    cppw::Sqlite3Connection* m_connection;
    std::string m_defaultFilter;
    std::unique_ptr<cppw::Sqlite3Statement> m_getSavedFiltersStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_insertFilterStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_insertFilterArgStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_updateDefaultStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_insertSortStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_sortNameStmt;
    std::unique_ptr<cppw::Sqlite3Statement> m_loadSortStmt;
    std::vector<colSort> m_sortingRules;
};
