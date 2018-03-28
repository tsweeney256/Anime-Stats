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

#include <wx/msgdlg.h>
#include "cppw/Sqlite3.hpp"
#include "DbFilter.hpp"

namespace FilterArg
{
    enum {
        watched = 0,
        watching,
        stalled,
        dropped,
        toWatch,
        blank,
        tv,
        movie,
        ova,
        ona,
        tvSpecial,
        releaseBlank,
        winter,
        spring,
        summer,
        fall,
        seasonBlank,
        ratingEnabled,
        yearEnabled,
        epsWatchedEnabled,
        epsRewatchedEnabled,
        totalEpsEnabled,
        lengthEnabled,
        dateStartedEnabled,
        dateFinishedEnabled,
        ratingLow,
        ratingHigh,
        yearLow,
        yearHigh,
        epsWatchedLow,
        epsWatchedHigh,
        epsRewatchedLow,
        epsRewatchedHigh,
        totalEpsLow,
        totalEpsHigh,
        lengthLow,
        lengthHigh,
        yearStartedLow,
        yearStartedHigh,
        monthStartedLow,
        monthStartedHigh,
        dayStartedLow,
        dayStartedHigh,
        yearFinishedLow,
        yearFinishedHigh,
        monthFinishedLow,
        monthFinishedHigh,
        dayFinishedLow,
        dayFinishedHigh
    };
}

static auto refBasicFilter = BasicFilterInfo::MakeUnique();
static auto refAdvFilter = AdvFilterInfo::MakeUnique();

DbFilter::DbFilter(cppw::Sqlite3Connection* connection)
{
    Reset(connection);
}

void DbFilter::Reset(cppw::Sqlite3Connection* connection)
{
    m_connection = connection;
    m_getSavedFiltersStmt = m_connection->PrepareStatement(
        "select SavedFilter.idSavedFilter, "
        "    SavedFilterArgName.idSavedFilterArgName, value "
        "from SavedFilter "
        "left join SavedFilterArg "
        "on SavedFilter.idSavedFilter = SavedFilterArg.idSavedFilter "
        "left join SavedFilterArgName "
        "on SavedFilterArgName.idSavedFilterArgName = "
        "    SavedFilterArg.idSavedFilterArgName "
        "where SavedFilter.name = ?");
    m_insertFilterStmt = m_connection->PrepareStatement(
        "insert into SavedFilter (`name`, `default`) values (?, ?)");
    m_insertFilterArgStmt = m_connection->PrepareStatement(
        "insert into SavedFilterArg "
        "(`idSavedFilter`, `idSavedFilterArgName`, `value`) "
        "values (?, ?, ?)");
    m_updateDefaultStmt = m_connection->PrepareStatement(
        "update SavedFilter set `default` = 0 "
        "where `idSavedFilter` <> ?");
    m_insertSortStmt = m_connection->PrepareStatement(
        "insert into SavedFilterSort "
        "(idSavedFilter, idSavedFilterSortName, asc) values (?, ?, ?)");
    m_sortNameStmt = m_connection->PrepareStatement(
        "select idSavedFilterSortName from SavedFilterSortName "
        "where `name` = ?");
    m_loadSortStmt = m_connection->PrepareStatement(
        "select `name`, `asc` from SavedFilterSort "
        "inner join SavedFilterSortName "
        "on SavedFilterSortName.idSavedFilterSortName = "
        "SavedFilterSort.idSavedFilterSortName "
        "where idSavedFilter = ? "
        "order by idSavedFilterSort asc");

    auto stmt = m_connection->PrepareStatement(
        "select name from SavedFilter where `default` = 1");
    auto result = stmt->GetResults();
    if (result->NextRow()) {
        m_defaultFilter = result->GetString(0);
    }
    LoadFilter(m_defaultFilter);
}

void DbFilter::LoadFilter(std::string name)
{
    auto idFilter = GetFilterFromDb(name);
    LoadSortFromDb(idFilter);
}

void DbFilter::SaveFilter(std::string name, bool makeDefault)
{
    auto idFilter = InsertFilterToDb(name, makeDefault);
    SaveSortToDb(idFilter);
    m_defaultFilter = name;
}

void DbFilter::DeleteFilter(std::string name)
{
    auto stmt = m_connection->PrepareStatement(
        "delete from `SavedFilter` where `name` = ?");
    stmt->Bind(1, name);
    auto result = stmt->GetResults();
    result->NextRow();
    if (name == m_defaultFilter) {
        m_defaultFilter = "";
    }
}

void DbFilter::SetDefaultFilter(std::string name)
{
    auto stmt = m_connection->PrepareStatement(
        "update SavedFilter set `default` = 1 where `name` = ?");
    stmt->Bind(1, name);
    auto result = stmt->GetResults();
    result->NextRow();
    stmt = m_connection->PrepareStatement(
        "update SavedFilter set `default` = 0 where `name` <> ?");
    stmt->Bind(1, name);
    result = stmt->GetResults();
    result->NextRow();
    m_defaultFilter = name;
}

std::vector<std::string> DbFilter::GetFilterNames()
{
    std::vector<std::string> ret;
    auto stmt = m_connection->PrepareStatement(
        "select name from SavedFilter "
        "order by name asc");
    auto result = stmt->GetResults();
    while (result->NextRow()) {
        ret.push_back(result->GetString(0));
    }
    return ret;
}

std::string DbFilter::GetDefaultFilterName()
{
    return m_defaultFilter;
}

Filter DbFilter::GetFilter()
{
    return Filter(m_basicFilterInfo.get(), m_advFilterInfo.get());
}

ConstFilter DbFilter::GetFilter() const
{
    return ConstFilter(m_basicFilterInfo.get(), m_advFilterInfo.get());
}

std::vector<colSort>* DbFilter::GetSort()
{
    const auto* cThis = this;
    return const_cast<std::vector<colSort>*>(cThis->GetSort());
}

const std::vector<colSort>* DbFilter::GetSort() const
{
    return &m_sortingRules;
}

void DbFilter::SetFilter(
    std::unique_ptr<BasicFilterInfo> bfi, std::unique_ptr<AdvFilterInfo> afi)
{
    m_basicFilterInfo = std::move(bfi);
    m_advFilterInfo = std::move(afi);
}

void DbFilter::SetSort(std::vector<colSort> sortingRules)
{
    m_sortingRules = sortingRules;
}

int DbFilter::GetFilterFromDb(std::string name)
{
    //a spectre is haunting this code, the spectre of horrible past designs
    m_basicFilterInfo = BasicFilterInfo::MakeUnique();
    m_advFilterInfo = AdvFilterInfo::MakeUnique();
    m_getSavedFiltersStmt->Reset();
    m_getSavedFiltersStmt->Bind(1, name);
    auto results = m_getSavedFiltersStmt->GetResults();
    //spooky scary ghosts ahead
    //were I a better person two years ago, I'd have used hash tables
    int idFilter = 0;
    while (results->NextRow()) {
        idFilter = results->GetInt(0);
        auto argId = results->GetInt(1);
        auto argVal = results->GetInt(2);
        if (results->IsNull(1)) {
            continue;
        }
        switch (argId) {
        case FilterArg::watched:
            m_basicFilterInfo->watched = argVal;
            break;
        case FilterArg::watching:
            m_basicFilterInfo->watching = argVal;
            break;
        case FilterArg::stalled:
            m_basicFilterInfo->stalled = argVal;
            break;
        case FilterArg::dropped:
            m_basicFilterInfo->dropped = argVal;
            break;
        case FilterArg::toWatch:
            m_basicFilterInfo->toWatch = argVal;
            break;
        case FilterArg::blank:
            m_basicFilterInfo->watchedBlank = argVal;
            break;
        case FilterArg::tv:
            m_advFilterInfo->tv = argVal;
            break;
        case FilterArg::movie:
            m_advFilterInfo->movie = argVal;
            break;
        case FilterArg::ova:
            m_advFilterInfo->ova = argVal;
            break;
        case FilterArg::ona:
            m_advFilterInfo->ona = argVal;
            break;
        case FilterArg::tvSpecial:
            m_advFilterInfo->tvSpecial = argVal;
            break;
        case FilterArg::releaseBlank:
            m_advFilterInfo->releaseBlank = argVal;
            break;
        case FilterArg::winter:
            m_advFilterInfo->winter = argVal;
            break;
        case FilterArg::spring:
            m_advFilterInfo->spring = argVal;
            break;
        case FilterArg::summer:
            m_advFilterInfo->summer = argVal;
            break;
        case FilterArg::fall:
            m_advFilterInfo->fall = argVal;
            break;
        case FilterArg::seasonBlank:
            m_advFilterInfo->seasonBlank = argVal;
            break;
        case FilterArg::ratingEnabled:
            m_advFilterInfo->ratingEnabled = argVal;
            break;
        case FilterArg::yearEnabled:
            m_advFilterInfo->yearEnabled = argVal;
            break;
        case FilterArg::epsWatchedEnabled:
            m_advFilterInfo->epsWatchedEnabled = argVal;
            break;
        case FilterArg::epsRewatchedEnabled:
            m_advFilterInfo->epsRewatchedEnabled = argVal;
            break;
        case FilterArg::totalEpsEnabled:
            m_advFilterInfo->totalEpsEnabled = argVal;
            break;
        case FilterArg::lengthEnabled:
            m_advFilterInfo->lengthEnabled = argVal;
            break;
        case FilterArg::dateStartedEnabled:
            m_advFilterInfo->dateStartedEnabled = argVal;
            break;
        case FilterArg::dateFinishedEnabled:
            m_advFilterInfo->dateFinishedEnabled = argVal;
            break;
        case FilterArg::ratingLow:
            m_advFilterInfo->ratingLow = argVal;
            break;
        case FilterArg::ratingHigh:
            m_advFilterInfo->ratingHigh = argVal;
            break;
        case FilterArg::yearLow:
            m_advFilterInfo->yearLow = argVal;
            break;
        case FilterArg::yearHigh:
            m_advFilterInfo->yearHigh = argVal;
            break;
        case FilterArg::epsWatchedLow:
            m_advFilterInfo->epsWatchedLow = argVal;
            break;
        case FilterArg::epsWatchedHigh:
            m_advFilterInfo->epsWatchedHigh = argVal;
            break;
        case FilterArg::epsRewatchedLow:
            m_advFilterInfo->epsRewatchedLow = argVal;
            break;
        case FilterArg::epsRewatchedHigh:
            m_advFilterInfo->epsRewatchedHigh = argVal;
            break;
        case FilterArg::totalEpsLow:
            m_advFilterInfo->totalEpsLow = argVal;
            break;
        case FilterArg::totalEpsHigh:
            m_advFilterInfo->totalEpsHigh = argVal;
            break;
        case FilterArg::lengthLow:
            m_advFilterInfo->lengthLow = argVal;
            break;
        case FilterArg::lengthHigh:
            m_advFilterInfo->lengthHigh = argVal;
            break;
        case FilterArg::yearStartedLow:
            m_advFilterInfo->yearStartedLow = argVal;
            break;
        case FilterArg::yearStartedHigh:
            m_advFilterInfo->yearStartedHigh = argVal;
            break;
        case FilterArg::monthStartedLow:
            m_advFilterInfo->monthStartedLow = argVal;
            break;
        case FilterArg::monthStartedHigh:
            m_advFilterInfo->monthStartedHigh = argVal;
            break;
        case FilterArg::dayStartedLow:
            m_advFilterInfo->dayStartedLow = argVal;
            break;
        case FilterArg::dayStartedHigh:
            m_advFilterInfo->dayStartedHigh = argVal;
            break;
        case FilterArg::yearFinishedLow:
            m_advFilterInfo->yearFinishedLow = argVal;
            break;
        case FilterArg::yearFinishedHigh:
            m_advFilterInfo->yearFinishedHigh = argVal;
            break;
        case FilterArg::monthFinishedLow:
            m_advFilterInfo->monthFinishedLow = argVal;
            break;
        case FilterArg::monthFinishedHigh:
            m_advFilterInfo->monthFinishedHigh = argVal;
            break;
        case FilterArg::dayFinishedLow:
            m_advFilterInfo->dayFinishedLow = argVal;
            break;
        case FilterArg::dayFinishedHigh:
            m_advFilterInfo->dayFinishedHigh = argVal;
            break;
        default:
            wxMessageBox(wxString("Illegal filter arg ID found: ") +
                         std::to_string(argId));
            break;
        }
    }
    return idFilter;
}

void DbFilter::InsertFilterArg(int idSavedFilterArgName, int value)
{
    m_insertFilterArgStmt->Reset();
    m_insertFilterArgStmt->Bind(2, idSavedFilterArgName);
    m_insertFilterArgStmt->Bind(3, value);
    auto result = m_insertFilterArgStmt->GetResults();
    result->NextRow();
}

int DbFilter::InsertFilterToDb(std::string name, bool makeDefault)
{
    m_insertFilterStmt->Reset();
    m_insertFilterStmt->Bind(1, name);
    m_insertFilterStmt->Bind(2, makeDefault);
    auto filter_results = m_insertFilterStmt->GetResults();
    filter_results->NextRow();
    auto idSavedFilter = m_connection->GetLastInsertRowID();

    if (makeDefault) {
        m_updateDefaultStmt->Reset();
        m_updateDefaultStmt->Bind(1, idSavedFilter);
        auto update_results = m_updateDefaultStmt->GetResults();
        update_results->NextRow();
    }

    const auto& b = *m_basicFilterInfo;
    const auto& rb = *refBasicFilter;
    const auto& a = *m_advFilterInfo;
    const auto& ra = *refAdvFilter;
    //more ghosts ahead
    //TODO: crash if fails. currently not exception safe
    m_insertFilterArgStmt->Reset();
    m_insertFilterArgStmt->Bind(1, idSavedFilter);
    if(b.watched != rb.watched) {
        InsertFilterArg(0, b.watched);
    }
    if(b.watching != rb.watching) {
        InsertFilterArg(1, b.watching);
    }
    if(b.stalled != rb.stalled) {
        InsertFilterArg(2, b.stalled);
    }
    if(b.dropped != rb.dropped) {
        InsertFilterArg(3, b.dropped);
    }
    if(b.toWatch != rb.toWatch) {
        InsertFilterArg(4, b.toWatch);
    }
    if(b.watchedBlank != rb.watchedBlank) {
        InsertFilterArg(5, b.watchedBlank);
    }
    if(a.tv != ra.tv) {
        InsertFilterArg(6, a.tv);
    }
    if(a.movie != ra.movie) {
        InsertFilterArg(7, a.movie);
    }
    if(a.ova != ra.ova) {
        InsertFilterArg(8, a.ova);
    }
    if(a.ona != ra.ona) {
        InsertFilterArg(9, a.ona);
    }
    if(a.tvSpecial != ra.tvSpecial) {
        InsertFilterArg(10, a.tvSpecial);
    }
    if(a.releaseBlank != ra.releaseBlank) {
        InsertFilterArg(11, a.releaseBlank);
    }
    if(a.winter != ra.winter) {
        InsertFilterArg(12, a.winter);
    }
    if(a.spring != ra.spring) {
        InsertFilterArg(13, a.spring);
    }
    if(a.summer != ra.summer) {
        InsertFilterArg(14, a.summer);
    }
    if(a.fall != ra.fall) {
        InsertFilterArg(15, a.fall);
    }
    if(a.seasonBlank != ra.seasonBlank) {
        InsertFilterArg(16, a.seasonBlank);
    }
    if(a.ratingEnabled != ra.ratingEnabled) {
        InsertFilterArg(17, a.ratingEnabled);
        if(a.ratingLow != ra.ratingLow) {
            InsertFilterArg(25, a.ratingLow);
        }
        if(a.ratingHigh != ra.ratingHigh) {
            InsertFilterArg(26, a.ratingHigh);
        }
    }
    if(a.yearEnabled != ra.yearEnabled) {
        InsertFilterArg(18, a.yearEnabled);
        if(a.yearLow != ra.yearLow) {
            InsertFilterArg(27, a.yearLow);
        }
        if(a.yearHigh != ra.yearHigh) {
            InsertFilterArg(28, a.yearHigh);
        }
    }
    if(a.epsWatchedEnabled != ra.epsWatchedEnabled) {
        InsertFilterArg(19, a.epsWatchedEnabled);
        if(a.epsWatchedLow != ra.epsWatchedLow) {
            InsertFilterArg(29, a.epsWatchedLow);
        }
        if(a.epsWatchedHigh != ra.epsWatchedHigh) {
            InsertFilterArg(30, a.epsWatchedHigh);
        }
    }
    if(a.epsRewatchedEnabled != ra.epsRewatchedEnabled) {
        InsertFilterArg(20, a.epsRewatchedEnabled);
        if(a.epsRewatchedLow != ra.epsRewatchedLow) {
            InsertFilterArg(31, a.epsRewatchedLow);
        }
        if(a.epsRewatchedHigh != ra.epsRewatchedHigh) {
            InsertFilterArg(32, a.epsRewatchedHigh);
        }
    }
    if(a.totalEpsEnabled != ra.totalEpsEnabled) {
        InsertFilterArg(21, a.totalEpsEnabled);
        if(a.totalEpsLow != ra.totalEpsLow) {
            InsertFilterArg(33, a.totalEpsLow);
        }
        if(a.totalEpsHigh != ra.totalEpsHigh) {
            InsertFilterArg(34, a.totalEpsHigh);
        }
    }
    if(a.lengthEnabled != ra.lengthEnabled) {
        InsertFilterArg(22, a.lengthEnabled);
        if(a.lengthLow != ra.lengthLow) {
            InsertFilterArg(35, a.lengthLow);
        }
        if(a.lengthHigh != ra.lengthHigh) {
            InsertFilterArg(36, a.lengthHigh);
        }
    }
    if(a.dateStartedEnabled != ra.dateStartedEnabled) {
        InsertFilterArg(23, a.dateStartedEnabled);
        if(a.yearStartedLow != ra.yearStartedLow) {
            InsertFilterArg(37, a.yearStartedLow);
        }
        if(a.yearStartedHigh != ra.yearStartedHigh) {
            InsertFilterArg(38, a.yearStartedHigh);
        }
        if(a.monthStartedLow != ra.monthStartedLow) {
            InsertFilterArg(39, a.monthStartedLow);
        }
        if(a.monthStartedHigh != ra.monthStartedHigh) {
            InsertFilterArg(40, a.monthStartedHigh);
        }
        if(a.dayStartedLow != ra.dayStartedLow) {
            InsertFilterArg(41, a.dayStartedLow);
        }
        if(a.dayStartedHigh != ra.dayStartedHigh) {
            InsertFilterArg(42, a.dayStartedHigh);
        }
    }
    if(a.dateFinishedEnabled != ra.dateFinishedEnabled) {
        InsertFilterArg(24, a.dateFinishedEnabled);
        if(a.yearFinishedLow != ra.yearFinishedLow) {
            InsertFilterArg(43, a.yearFinishedLow);
        }
        if(a.yearFinishedHigh != ra.yearFinishedHigh) {
            InsertFilterArg(44, a.yearFinishedHigh);
        }
        if(a.monthFinishedLow != ra.monthFinishedLow) {
            InsertFilterArg(45, a.monthFinishedLow);
        }
        if(a.monthFinishedHigh != ra.monthFinishedHigh) {
            InsertFilterArg(46, a.monthFinishedHigh);
        }
        if(a.dayFinishedLow != ra.dayFinishedLow) {
            InsertFilterArg(47, a.dayFinishedLow);
        }
        if(a.dayFinishedHigh != ra.dayFinishedHigh) {
            InsertFilterArg(48, a.dayFinishedHigh);
        }
    }
    return idSavedFilter;
}

void DbFilter::SaveSortEntryToDb(colSort sortingRule)
{
    m_sortNameStmt->Reset();
    m_sortNameStmt->Bind(1, sortingRule.name.utf8_str());
    auto sortNameResult = m_sortNameStmt->GetResults();
    sortNameResult->NextRow();
    auto idSavedFilterSortName = sortNameResult->GetInt(0);
    m_insertSortStmt->Reset();
    m_insertSortStmt->Bind(2, idSavedFilterSortName);
    m_insertSortStmt->Bind(3, sortingRule.asc);
    auto insertResult = m_insertSortStmt->GetResults();
    insertResult->NextRow();
}

void DbFilter::SaveSortToDb(int idSavedFilter)
{
    m_insertSortStmt->Reset();
    m_insertSortStmt->Bind(1, idSavedFilter);
    for (auto rule : m_sortingRules) {
        SaveSortEntryToDb(rule);
    }
}

void DbFilter::LoadSortFromDb(int idSavedFilter)
{
    m_loadSortStmt->Reset();
    m_loadSortStmt->Bind(1, idSavedFilter);
    auto result = m_loadSortStmt->GetResults();
    m_sortingRules.clear();
    bool savedSort = false;
    while (result->NextRow()) {
        savedSort = true;
        m_sortingRules.emplace_back(result->GetString(0), result->GetBool(1));
    }
    if (!savedSort) {
        m_sortingRules.emplace_back("nameSort", true);
    }
}
