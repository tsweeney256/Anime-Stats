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

#include <fmt/format.h>
#include "SqlStrings.hpp"

using namespace fmt::literals;

namespace SqlStrings{
    const std::string distribution =
        "select {group_col},\n"
        "    printf(\"%.2f\", avg(Rating)) as Average,\n"
        "    printf(\"%.2f\", median(Rating)) as Median,\n"
        "    printf(\"%.2f\", stdev(Rating)) as `Std. Dev`,\n"
        "    count(Rating) as `Count`\n"
        "from {from_table}\n"
        "group by {group_col}\n"
        "{having_expr}\n"
        "{order_by} {group_col}\n";

    const std::string countWatchedStatus =
        "select {group_col},\n"
        "    count(idWatchedStatus, 1) as Watched,\n"
        "    count(idWatchedStatus, 2) as Watching,\n"
        "    count(idWatchedStatus, 3) as Stalled,\n"
        "    count(idWatchedStatus, 4) as Dropped,\n"
        "    count(idWatchedStatus, 5) as `To Watch`\n"
        "from {from_table}\n"
        "group by {group_col}\n"
        "{having_expr}\n"
        "{order_by} {group_col}\n";


    const std::string countReleaseType =
        "select {group_col},\n"
        "    count(idReleaseType, 1) as TV,\n"
        "    count(idReleaseType, 2) as OVA,\n"
        "    count(idReleaseType, 3) as ONA,\n"
        "    count(idReleaseType, 4) as Movie,\n"
        "    count(idReleaseType, 5) as `TV Special`\n"
        "from {from_table}\n"
        "group by {group_col}\n"
        "{having_expr}\n"
        "{order_by} {group_col}\n";

    const std::string timeWatchedSql =
        "--There are 366.2422 stellar days in a year\n"
        "--Dividing by 12 gives us 30.520183333 days in a month\n"
        "--A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "select {group_col}, printf(\"%.2f\", totalDays) as `Days`, printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  watchedYear, month, day, hour, minute ) as `Formatted`, '' as `With Rewatches`, printf(\"%.2f\",  totalRewatchedDays) as `Days`, printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedHour, rewatchedMinute) as `Formatted`\n"
        "from(select {group_col}, totalDays, watchedYear, month, day, hour, val * 60 as minute, val, totalRewatchedDays, rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedHour, rewatchedVal * 60 as rewatchedMinute, rewatchedVal\n"
        "    from(select {group_col}, totalDays, val * 24 - cast(val * 24 as int) as val, watchedYear, month, day, val * 24 as hour, totalRewatchedDays, rewatchedVal * 24 - cast(rewatchedVal * 24 as int) as rewatchedVal, rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedVal * 24 as rewatchedHour\n"
        "        from (select {group_col}, totalDays, val - cast(val as int) as val, watchedYear, month, val as day, totalRewatchedDays, rewatchedVal - cast(rewatchedVal as int) as rewatchedVal, rewatchedYear, rewatchedMonth, rewatchedVal as rewatchedDay\n"
        "            from (select {group_col}, totalDays, val - cast(val / 30.520183333 as int) * 30.520183333 as val, watchedYear, val / 30.520183333 as month, totalRewatchedDays, rewatchedVal - cast(rewatchedVal / 30.520183333 as int) * 30.520183333 as rewatchedVal, rewatchedYear, rewatchedVal / 30.520183333 as rewatchedMonth\n"
        "                from (select {group_col}, val as totalDays, val - cast(val / 366.2422 as int)  * 366.2422 as val, val / 366.2422  as watchedYear, rewatchedVal as totalRewatchedDays, rewatchedVal - cast(rewatchedVal / 366.2422 as int)  * 366.2422 as rewatchedVal, rewatchedVal / 366.2422  as rewatchedYear\n"
        "                    from (select {group_col}, sum(episodesWatched * episodeLength) / 1436.068333333 as val, sum((ifnull(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333 as rewatchedVal from {from_table} group by {group_col} {having_expr}))))))\n"
        "{order_by} {group_col}\n";

    //ugly amalgamation of the above to make sorting easier
    const std::string amalgamationColsBottom =
        //Amount Watched
        /*these columns are baked into the query because timeWatched is
         more complicated*/
        //Rating
        "    cast(printf(\"%.2f\", avg(Rating)) as float) as Average,\n"
        "    cast(printf(\"%.2f\", median(Rating)) as float) as Median,\n"
        "    cast(printf(\"%.2f\", stdev(Rating)) as float) as `Std. Dev`,\n"
        "    count(Rating) as `Count`,\n"
        //Count Watched Status
        "    count(idWatchedStatus, 1) as Watched,\n"
        "    count(idWatchedStatus, 2) as Watching,\n"
        "    count(idWatchedStatus, 3) as Stalled,\n"
        "    count(idWatchedStatus, 4) as Dropped,\n"
        "    count(idWatchedStatus, 5) as `To Watch`,\n"
        //Count Release Type
        "    count(idReleaseType, 1) as TV,\n"
        "    count(idReleaseType, 2) as OVA,\n"
        "    count(idReleaseType, 3) as ONA,\n"
        "    count(idReleaseType, 4) as Movie,\n"
        "    count(idReleaseType, 5) as `TV Special`\n";

    const std::string amalgamationColsTop =
        "Average, Median, `Std. Dev`, `Count`, Watched, Watching, `Stalled`, Dropped, `To Watch`, TV, OVA, ONA, Movie, `TV Special`";

    const std::string bigUglyAmalgamation =
        "--There are 366.2422 stellar days in a year\n"
        "--Dividing by 12 gives us 30.520183333 days in a month\n"
        "--A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "select `{group_col}`,\n"
        "    cast(printf(\"%.2f\", totalDays) as float) as `Days`,\n"
        "    printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  watchedYear, month, day, hour, minute ) as `Formatted`,\n"
        "    cast(printf(\"%.2f\",  totalRewatchedDays) as float) as `Days with Rewatched`,\n"
        "    printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedHour, rewatchedMinute) as `Formatted`,\n"
        //wanted to use fmt::format to make this prettier but it doesnt
        //support partial formatting
        + amalgamationColsTop +
        "from(select `{group_col}`, totalDays, watchedYear, month, day, hour, val * 60 as minute, val, totalRewatchedDays, rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedHour, rewatchedVal * 60 as rewatchedMinute, rewatchedVal, " + amalgamationColsTop + "\n"
        "    from(select `{group_col}`, totalDays, val * 24 - cast(val * 24 as int) as val, watchedYear, month, day, val * 24 as hour, totalRewatchedDays, rewatchedVal * 24 - cast(rewatchedVal * 24 as int) as rewatchedVal, rewatchedYear, rewatchedMonth, rewatchedDay, rewatchedVal * 24 as rewatchedHour, " + amalgamationColsTop + "\n"
        "        from (select `{group_col}`, totalDays, val - cast(val as int) as val, watchedYear, month, val as day, totalRewatchedDays, rewatchedVal - cast(rewatchedVal as int) as rewatchedVal, rewatchedYear, rewatchedMonth, rewatchedVal as rewatchedDay, " + amalgamationColsTop + "\n"
        "            from (select `{group_col}`, totalDays, val - cast(val / 30.520183333 as int) * 30.520183333 as val, watchedYear, val / 30.520183333 as month, totalRewatchedDays, rewatchedVal - cast(rewatchedVal / 30.520183333 as int) * 30.520183333 as rewatchedVal, rewatchedYear, rewatchedVal / 30.520183333 as rewatchedMonth, " + amalgamationColsTop + "\n"
        "                from (select `{group_col}`, val as totalDays, val - cast(val / 366.2422 as int)  * 366.2422 as val, val / 366.2422  as watchedYear, rewatchedVal as totalRewatchedDays, rewatchedVal - cast(rewatchedVal / 366.2422 as int)  * 366.2422 as rewatchedVal, rewatchedVal / 366.2422  as rewatchedYear, " + amalgamationColsTop + "\n"
        "                    from (\n"
        "                        select `{group_col}`,\n"
        "                        sum(episodesWatched * episodeLength) / 1436.068333333 as val,\n"
        "                        sum((ifnull(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333 as rewatchedVal,\n"
        + amalgamationColsBottom +
        "                        from {from_table}\n"
        "                        group by `{group_col}`\n"
        "                        {having_expr}\n"
        "))))))\n"
        "order by `{order_col}` {order_direction}\n";
}
