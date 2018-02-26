/*Anime Stats
  Copyright (C) 2016 Thomas Sweeney
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

#ifndef FILTERSTRUCTS_HPP
#define FILTERSTRUCTS_HPP

#include <memory>
#include <tuple>

namespace cppw {
    class Sqlite3Connection;
}

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

struct BasicFilterInfo
{
    static std::unique_ptr<BasicFilterInfo> MakeUnique()
        { return std::unique_ptr<BasicFilterInfo>(new BasicFilterInfo()); }
    static std::shared_ptr<BasicFilterInfo> MakeShared()
        { return std::shared_ptr<BasicFilterInfo>(new BasicFilterInfo()); }

    bool watched = true;
    bool watching = true;
    bool stalled = true;
    bool dropped = true;
    bool watchedBlank = true;
    bool toWatch = true;
    std::string title;

private:
    BasicFilterInfo() {}
};

struct AdvFilterInfo
{
    static std::unique_ptr<AdvFilterInfo> MakeUnique()
        { return std::unique_ptr<AdvFilterInfo>(new AdvFilterInfo()); }
    static std::shared_ptr<AdvFilterInfo> MakeShared()
        { return std::shared_ptr<AdvFilterInfo>(new AdvFilterInfo()); }

    bool tv = true;
    bool movie = true;
    bool ova = true;
    bool ona = true;
    bool tvSpecial = true;
    bool releaseBlank = true;

    bool winter = true;
    bool spring = true;
    bool summer = true;
    bool fall = true;
    bool seasonBlank = true;

    bool ratingEnabled = false;
    bool yearEnabled = false;
    bool epsWatchedEnabled = false;
    bool epsRewatchedEnabled = false;
    bool totalEpsEnabled = false;
    bool lengthEnabled = false;
    bool dateStartedEnabled = false;
    bool dateFinishedEnabled = false;

    int ratingLow = 0;
    int ratingHigh = 0;
    int yearLow = 0;
    int yearHigh = 0;
    int epsWatchedLow = 0;
    int epsWatchedHigh = 0;
    int epsRewatchedLow = 0;
    int epsRewatchedHigh = 0;
    int totalEpsLow = 0;
    int totalEpsHigh = 0;
    int lengthLow = 0;
    int lengthHigh = 0;

    int yearStartedLow = 0;
    int yearStartedHigh = 0;
    int monthStartedLow = 1;
    int monthStartedHigh = 1;
    int dayStartedLow = 1;
    int dayStartedHigh = 1;

    int yearFinishedLow = 0;
    int yearFinishedHigh = 0;
    int monthFinishedLow = 1;
    int monthFinishedHigh = 1;
    int dayFinishedLow = 1;
    int dayFinishedHigh = 1;

private:
    AdvFilterInfo() {}
};

using FilterTuple = std::tuple<std::shared_ptr<BasicFilterInfo>,
                               std::shared_ptr<AdvFilterInfo>>;

FilterTuple GetFiltersFromDb(cppw::Sqlite3Connection* connection,
                 const wxString& filterName, const wxString& title);

int InsertFiltersToDb(cppw::Sqlite3Connection* connection,
                       const wxString& filterName,
                       bool defaultFilter,
                       BasicFilterInfo* basicFilterInfo,
                       AdvFilterInfo* advFilterInfo);

#endif
