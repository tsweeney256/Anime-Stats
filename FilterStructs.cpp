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
    while (results->NextRow()) {
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
    return FilterTuple(basicFilterInfo, advFilterInfo);
}
