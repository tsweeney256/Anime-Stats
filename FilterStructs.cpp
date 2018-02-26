#include <wx/msgdlg.h>
#include "cppw/Sqlite3.hpp"
#include "FilterStructs.hpp"

static std::unique_ptr<cppw::Sqlite3Statement> select_stmt = nullptr;

FilterTuple GetFiltersFromDb(cppw::Sqlite3Connection* connection,
                             const wxString& filterName,
                             const wxString& title) {
    //a spectre is haunting this code, the spectre of horrible past designs
    auto basicFilterInfo = BasicFilterInfo::MakeShared();
    basicFilterInfo->title =
        std::string(title.utf8_str());
    auto advFilterInfo = AdvFilterInfo::MakeShared();
    if (!select_stmt) {
        select_stmt = connection->PrepareStatement(
            "select SavedFilter.idSavedFilter, "
            "    SavedFilterArgName.idSavedFilterArgName, value "
            "from SavedFilter "
            "left join SavedFilterArg "
            "on SavedFilter.idSavedFilter = SavedFilterArg.idSavedFilter "
            "left join SavedFilterArgName "
            "on SavedFilterArgName.idSavedFilterArgName = "
            "    SavedFilterArg.idSavedFilterArgName "
            "where SavedFilter.name = ?");
    }
    select_stmt->Reset();
    select_stmt->Bind(1, filterName.utf8_str());
    auto results = select_stmt->GetResults();
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
            basicFilterInfo->watched = argVal;
            break;
        case FilterArg::watching:
            basicFilterInfo->watching = argVal;
            break;
        case FilterArg::stalled:
            basicFilterInfo->stalled = argVal;
            break;
        case FilterArg::dropped:
            basicFilterInfo->dropped = argVal;
            break;
        case FilterArg::toWatch:
            basicFilterInfo->toWatch = argVal;
            break;
        case FilterArg::blank:
            basicFilterInfo->watchedBlank = argVal;
            break;
        case FilterArg::tv:
            advFilterInfo->tv = argVal;
            break;
        case FilterArg::movie:
            advFilterInfo->movie = argVal;
            break;
        case FilterArg::ova:
            advFilterInfo->ova = argVal;
            break;
        case FilterArg::ona:
            advFilterInfo->ona = argVal;
            break;
        case FilterArg::tvSpecial:
            advFilterInfo->tvSpecial = argVal;
            break;
        case FilterArg::releaseBlank:
            advFilterInfo->releaseBlank = argVal;
            break;
        case FilterArg::winter:
            advFilterInfo->winter = argVal;
            break;
        case FilterArg::spring:
            advFilterInfo->spring = argVal;
            break;
        case FilterArg::summer:
            advFilterInfo->summer = argVal;
            break;
        case FilterArg::fall:
            advFilterInfo->fall = argVal;
            break;
        case FilterArg::seasonBlank:
            advFilterInfo->seasonBlank = argVal;
            break;
        case FilterArg::ratingEnabled:
            advFilterInfo->ratingEnabled = argVal;
            break;
        case FilterArg::yearEnabled:
            advFilterInfo->yearEnabled = argVal;
            break;
        case FilterArg::epsWatchedEnabled:
            advFilterInfo->epsWatchedEnabled = argVal;
            break;
        case FilterArg::epsRewatchedEnabled:
            advFilterInfo->epsRewatchedEnabled = argVal;
            break;
        case FilterArg::totalEpsEnabled:
            advFilterInfo->totalEpsEnabled = argVal;
            break;
        case FilterArg::lengthEnabled:
            advFilterInfo->lengthEnabled = argVal;
            break;
        case FilterArg::dateStartedEnabled:
            advFilterInfo->dateStartedEnabled = argVal;
            break;
        case FilterArg::dateFinishedEnabled:
            advFilterInfo->dateFinishedEnabled = argVal;
            break;
        case FilterArg::ratingLow:
            advFilterInfo->ratingLow = argVal;
            break;
        case FilterArg::ratingHigh:
            advFilterInfo->ratingHigh = argVal;
            break;
        case FilterArg::yearLow:
            advFilterInfo->yearLow = argVal;
            break;
        case FilterArg::yearHigh:
            advFilterInfo->yearHigh = argVal;
            break;
        case FilterArg::epsWatchedLow:
            advFilterInfo->epsWatchedLow = argVal;
            break;
        case FilterArg::epsWatchedHigh:
            advFilterInfo->epsWatchedHigh = argVal;
            break;
        case FilterArg::epsRewatchedLow:
            advFilterInfo->epsRewatchedLow = argVal;
            break;
        case FilterArg::epsRewatchedHigh:
            advFilterInfo->epsRewatchedHigh = argVal;
            break;
        case FilterArg::totalEpsLow:
            advFilterInfo->totalEpsLow = argVal;
            break;
        case FilterArg::totalEpsHigh:
            advFilterInfo->totalEpsHigh = argVal;
            break;
        case FilterArg::lengthLow:
            advFilterInfo->lengthLow = argVal;
            break;
        case FilterArg::lengthHigh:
            advFilterInfo->lengthHigh = argVal;
            break;
        case FilterArg::yearStartedLow:
            advFilterInfo->yearStartedLow = argVal;
            break;
        case FilterArg::yearStartedHigh:
            advFilterInfo->yearStartedHigh = argVal;
            break;
        case FilterArg::monthStartedLow:
            advFilterInfo->monthStartedLow = argVal;
            break;
        case FilterArg::monthStartedHigh:
            advFilterInfo->monthStartedHigh = argVal;
            break;
        case FilterArg::dayStartedLow:
            advFilterInfo->dayStartedLow = argVal;
            break;
        case FilterArg::dayStartedHigh:
            advFilterInfo->dayStartedHigh = argVal;
            break;
        case FilterArg::yearFinishedLow:
            advFilterInfo->yearFinishedLow = argVal;
            break;
        case FilterArg::yearFinishedHigh:
            advFilterInfo->yearFinishedHigh = argVal;
            break;
        case FilterArg::monthFinishedLow:
            advFilterInfo->monthFinishedLow = argVal;
            break;
        case FilterArg::monthFinishedHigh:
            advFilterInfo->monthFinishedHigh = argVal;
            break;
        case FilterArg::dayFinishedLow:
            advFilterInfo->dayFinishedLow = argVal;
            break;
        case FilterArg::dayFinishedHigh:
            advFilterInfo->dayFinishedHigh = argVal;
            break;
        default:
            wxMessageBox(wxString("Illegal filter arg ID found: ") +
                         std::to_string(argId));
            break;
        }
    }
    return FilterTuple(idFilter, basicFilterInfo, advFilterInfo);
}

static std::unique_ptr<cppw::Sqlite3Statement> filter_stmt = nullptr;
static std::unique_ptr<cppw::Sqlite3Statement> arg_stmt = nullptr;
static auto refBasicFilter = BasicFilterInfo::MakeUnique();
static auto refAdvFilter = AdvFilterInfo::MakeUnique();

static void InsertFilterArg(cppw::Sqlite3Statement* stmt,
                          int idSavedFilterArgName,
                          int value)
{
    stmt->Reset();
    stmt->Bind(2, idSavedFilterArgName);
    stmt->Bind(3, value);
    auto result = stmt->GetResults();
    result->NextRow();
}

int InsertFiltersToDb(cppw::Sqlite3Connection* connection,
                       const wxString& filterName,
                       bool defaultFilter,
                       BasicFilterInfo* basicFilterInfo,
                       AdvFilterInfo* advFilterInfo)
{
    if (!filter_stmt) {
        filter_stmt = connection->PrepareStatement(
            "insert into SavedFilter (`name`, `default`) values (?, ?)");
    }
    filter_stmt->Reset();
    filter_stmt->Bind(1, filterName.utf8_str());
    filter_stmt->Bind(2, defaultFilter);
    auto filter_results = filter_stmt->GetResults();
    filter_results->NextRow();
    auto idSavedFilter = connection->GetLastInsertRowID();

    if (!arg_stmt) {
        arg_stmt = connection->PrepareStatement(
            "insert into SavedFilterArg "
            "(`idSavedFilter`, `idSavedFilterArgName`, `value`) "
            "values (?, ?, ?)");
    }

    const auto& b = *basicFilterInfo;
    const auto& rb = *refBasicFilter;
    const auto& a = *advFilterInfo;
    const auto& ra = *refAdvFilter;
    //more ghosts ahead
    arg_stmt->Reset();
    arg_stmt->Bind(1, idSavedFilter);
    if(b.watched != rb.watched) {
        InsertFilterArg(arg_stmt.get(), 0, b.watched);
    }
    if(b.watching != rb.watching) {
        InsertFilterArg(arg_stmt.get(), 1, b.watching);
    }
    if(b.stalled != rb.stalled) {
        InsertFilterArg(arg_stmt.get(), 2, b.stalled);
    }
    if(b.dropped != rb.dropped) {
        InsertFilterArg(arg_stmt.get(), 3, b.dropped);
    }
    if(b.toWatch != rb.toWatch) {
        InsertFilterArg(arg_stmt.get(), 4, b.toWatch);
    }
    if(b.watchedBlank != rb.watchedBlank) {
        InsertFilterArg(arg_stmt.get(), 5, b.watchedBlank);
    }
    if(a.tv != ra.tv) {
        InsertFilterArg(arg_stmt.get(), 6, a.tv);
    }
    if(a.movie != ra.movie) {
        InsertFilterArg(arg_stmt.get(), 7, a.movie);
    }
    if(a.ova != ra.ova) {
        InsertFilterArg(arg_stmt.get(), 8, a.ova);
    }
    if(a.ona != ra.ona) {
        InsertFilterArg(arg_stmt.get(), 9, a.ona);
    }
    if(a.tvSpecial != ra.tvSpecial) {
        InsertFilterArg(arg_stmt.get(), 10, a.tvSpecial);
    }
    if(a.releaseBlank != ra.releaseBlank) {
        InsertFilterArg(arg_stmt.get(), 11, a.releaseBlank);
    }
    if(a.winter != ra.winter) {
        InsertFilterArg(arg_stmt.get(), 12, a.winter);
    }
    if(a.spring != ra.spring) {
        InsertFilterArg(arg_stmt.get(), 13, a.spring);
    }
    if(a.summer != ra.summer) {
        InsertFilterArg(arg_stmt.get(), 14, a.summer);
    }
    if(a.fall != ra.fall) {
        InsertFilterArg(arg_stmt.get(), 15, a.fall);
    }
    if(a.seasonBlank != ra.seasonBlank) {
        InsertFilterArg(arg_stmt.get(), 16, a.seasonBlank);
    }
    if(a.ratingEnabled != ra.ratingEnabled) {
        InsertFilterArg(arg_stmt.get(), 17, a.ratingEnabled);
        if(a.ratingLow != ra.ratingLow) {
            InsertFilterArg(arg_stmt.get(), 25, a.ratingLow);
        }
        if(a.ratingHigh != ra.ratingHigh) {
            InsertFilterArg(arg_stmt.get(), 26, a.ratingHigh);
        }
    }
    if(a.yearEnabled != ra.yearEnabled) {
        InsertFilterArg(arg_stmt.get(), 18, a.yearEnabled);
        if(a.yearLow != ra.yearLow) {
            InsertFilterArg(arg_stmt.get(), 27, a.yearLow);
        }
        if(a.yearHigh != ra.yearHigh) {
            InsertFilterArg(arg_stmt.get(), 28, a.yearHigh);
        }
    }
    if(a.epsWatchedEnabled != ra.epsWatchedEnabled) {
        InsertFilterArg(arg_stmt.get(), 19, a.epsWatchedEnabled);
        if(a.epsWatchedLow != ra.epsWatchedLow) {
            InsertFilterArg(arg_stmt.get(), 29, a.epsWatchedLow);
        }
        if(a.epsWatchedHigh != ra.epsWatchedHigh) {
            InsertFilterArg(arg_stmt.get(), 30, a.epsWatchedHigh);
        }
    }
    if(a.epsRewatchedEnabled != ra.epsRewatchedEnabled) {
        InsertFilterArg(arg_stmt.get(), 20, a.epsRewatchedEnabled);
        if(a.epsRewatchedLow != ra.epsRewatchedLow) {
            InsertFilterArg(arg_stmt.get(), 31, a.epsRewatchedLow);
        }
        if(a.epsRewatchedHigh != ra.epsRewatchedHigh) {
            InsertFilterArg(arg_stmt.get(), 32, a.epsRewatchedHigh);
        }
    }
    if(a.totalEpsEnabled != ra.totalEpsEnabled) {
        InsertFilterArg(arg_stmt.get(), 21, a.totalEpsEnabled);
        if(a.totalEpsLow != ra.totalEpsLow) {
            InsertFilterArg(arg_stmt.get(), 33, a.totalEpsLow);
        }
        if(a.totalEpsHigh != ra.totalEpsHigh) {
            InsertFilterArg(arg_stmt.get(), 34, a.totalEpsHigh);
        }
    }
    if(a.lengthEnabled != ra.lengthEnabled) {
        InsertFilterArg(arg_stmt.get(), 22, a.lengthEnabled);
        if(a.lengthLow != ra.lengthLow) {
            InsertFilterArg(arg_stmt.get(), 35, a.lengthLow);
        }
        if(a.lengthHigh != ra.lengthHigh) {
            InsertFilterArg(arg_stmt.get(), 36, a.lengthHigh);
        }
    }
    if(a.dateStartedEnabled != ra.dateStartedEnabled) {
        InsertFilterArg(arg_stmt.get(), 23, a.dateStartedEnabled);
        if(a.yearStartedLow != ra.yearStartedLow) {
            InsertFilterArg(arg_stmt.get(), 37, a.yearStartedLow);
        }
        if(a.yearStartedHigh != ra.yearStartedHigh) {
            InsertFilterArg(arg_stmt.get(), 38, a.yearStartedHigh);
        }
        if(a.monthStartedLow != ra.monthStartedLow) {
            InsertFilterArg(arg_stmt.get(), 39, a.monthStartedLow);
        }
        if(a.monthStartedHigh != ra.monthStartedHigh) {
            InsertFilterArg(arg_stmt.get(), 40, a.monthStartedHigh);
        }
        if(a.dayStartedLow != ra.dayStartedLow) {
            InsertFilterArg(arg_stmt.get(), 41, a.dayStartedLow);
        }
        if(a.dayStartedHigh != ra.dayStartedHigh) {
            InsertFilterArg(arg_stmt.get(), 42, a.dayStartedHigh);
        }
    }
    if(a.dateFinishedEnabled != ra.dateFinishedEnabled) {
        InsertFilterArg(arg_stmt.get(), 24, a.dateFinishedEnabled);
        if(a.yearFinishedLow != ra.yearFinishedLow) {
            InsertFilterArg(arg_stmt.get(), 43, a.yearFinishedLow);
        }
        if(a.yearFinishedHigh != ra.yearFinishedHigh) {
            InsertFilterArg(arg_stmt.get(), 44, a.yearFinishedHigh);
        }
        if(a.monthFinishedLow != ra.monthFinishedLow) {
            InsertFilterArg(arg_stmt.get(), 45, a.monthFinishedLow);
        }
        if(a.monthFinishedHigh != ra.monthFinishedHigh) {
            InsertFilterArg(arg_stmt.get(), 46, a.monthFinishedHigh);
        }
        if(a.dayFinishedLow != ra.dayFinishedLow) {
            InsertFilterArg(arg_stmt.get(), 47, a.dayFinishedLow);
        }
        if(a.dayFinishedHigh != ra.dayFinishedHigh) {
            InsertFilterArg(arg_stmt.get(), 48, a.dayFinishedHigh);
        }
    }
    return idSavedFilter;
}
