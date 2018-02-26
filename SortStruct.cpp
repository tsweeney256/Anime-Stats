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

#include <memory>
#include "cppw/Sqlite3.hpp"
#include "SortStruct.hpp"

static std::unique_ptr<cppw::Sqlite3Statement> insertStmt = nullptr;
static std::unique_ptr<cppw::Sqlite3Statement> sortNameStmt = nullptr;

static void SaveSortEntryToDb(colSort sortingRule)
{
    sortNameStmt->Reset();
    sortNameStmt->Bind(1, sortingRule.name.utf8_str());
    auto sortNameResult = sortNameStmt->GetResults();
    sortNameResult->NextRow();
    auto idSavedFilterSortName = sortNameResult->GetInt(0);
    insertStmt->Reset();
    insertStmt->Bind(2, idSavedFilterSortName);
    insertStmt->Bind(3, sortingRule.asc);
    auto insertResult = insertStmt->GetResults();
    insertResult->NextRow();
}

void SaveSortToDb(cppw::Sqlite3Connection* connection,
                  int idSavedFilter, std::vector<colSort> sortingRules)
{
    if (!insertStmt) {
        insertStmt = connection->PrepareStatement(
            "insert into SavedFilterSort "
            "(idSavedFilter, idSavedFilterSortName, asc) values (?, ?, ?)");
    }
    if (!sortNameStmt) {
        sortNameStmt = connection->PrepareStatement(
            "select idSavedFilterSortName from SavedFilterSortName "
            "where `name` = ?");
    }
    insertStmt->Reset();
    insertStmt->Bind(1, idSavedFilter);
    for (auto rule : sortingRules) {
        SaveSortEntryToDb(rule);
    }
}
