#include "SqlStrings.hpp"

namespace SqlStrings{
    const std::string medianStr =
        "select median({0})\n"
        "from Series\n";

    const std::string distribution =
        "select\n"
        "    printf(\"%.2f\", avg(rating)) as `Average`,\n"
        "    printf(\"%.2f\", median(rating)) as `Median`,\n"
        "    printf(\"%.2f\", stdev(rating)) as `Std. Dev`,\n"
        "    count(rating) as `Count`\n"
        "from Series\n";

    const std::string countWatchedStatus =
        "select `Watched`, `Watching`, `Stalled`, `Dropped`, `To Watch`\n"
        "from (\n"
        "    select count(idWatchedStatus) as `Watched`\n"
        "    from Series\n"
        "    where idWatchedStatus = 1\n"
        ")\n"
        "left join (\n"
        "    select count(idWatchedStatus) as `Watching`\n"
        "    from Series\n"
        "    where idWatchedStatus = 2\n"
        ")\n"
        "left join (\n"
        "    select count(idWatchedStatus) as `Stalled`\n"
        "    from Series\n"
        "    where idWatchedStatus = 3\n"
        ")\n"
        "left join (\n"
        "    select count(idWatchedStatus) as `Dropped`\n"
        "    from Series\n"
        "    where idWatchedStatus = 4\n"
        ")\n"
        "left join (\n"
        "    select count(idWatchedStatus) as `To Watch`\n"
        "    from Series\n"
        "    where idWatchedStatus = 5\n"
        ")\n";

    const std::string countReleaseType =
        "select `TV`, `OVA`, `ONA`, `Movie`, `TV Special`\n"
        "from (\n"
        "    select count(idReleaseType) as `TV`\n"
        "    from Series\n"
        "    where idReleaseType = 1\n"
        ")\n"
        "left join (\n"
        "    select count(idReleaseType) as `OVA`\n"
        "    from Series\n"
        "    where idReleaseType = 2\n"
        ")\n"
        "left join (\n"
        "    select count(idReleaseType) as `ONA`\n"
        "    from Series\n"
        "    where idReleaseType = 3\n"
        ")\n"
        "left join (\n"
        "    select count(idReleaseType) as `Movie`\n"
        "    from Series\n"
        "    where idReleaseType = 4\n"
        ")\n"
        "left join (\n"
        "    select count(idReleaseType) as `TV Special`\n"
        "    from Series\n"
        "    where idReleaseType = 5\n"
        ")\n";

    const std::string timeWatchedSql =
        "SELECT printf(\"%.2f\",  sum(episodesWatched * episodeLength) / 1436.068333333) as `Days`, `Formatted`, '' as `With Rewatches`, `third` as `Days`, `fourth` as `Formatted`\n"
        "from Series\n"
        "left join(\n"
        "    SELECT printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  year, month, day, hour, minute ) as `Formatted`\n"
        "    FROM(SELECT year, month, day, hour, val * 60 as minute\n"
        "        FROM(SELECT val * 24 - CAST(val * 24 as INT) as val, year, month, day, val * 24 as hour\n"
        "            FROM (SELECT val - CAST(val as INT) as val, year, month, val as day\n"
        "                FROM (SELECT val - CAST(val / 30.520183333 as INT) * 30.520183333 as val, year, val / 30.520183333 as month\n"
        "                    FROM (SELECT val - CAST(val / 366.2422 as INT)  * 366.2422 as val, val / 366.2422  as year\n"
        "                        FROM (SELECT sum(episodesWatched * episodeLength) / 1436.068333333 as val FROM Series)))))))\n"
        "left join(\n"
        "    SELECT printf(\"%.2f\",  sum((IFNULL(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333) as `third`\n"
        "    from Series)\n"
        "left join(\n"
        "       --There are 366.2422 stellar days in a year\n"
        "       --Dividing by 12 gives us 30.520183333 days in a month\n"
        "       --A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "       SELECT printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  year, month, day, hour, minute ) as `fourth`\n"
        "       FROM(SELECT year, month, day, hour, val * 60 as minute\n"
        "               FROM(SELECT val * 24 - CAST(val * 24 as INT) as val, year, month, day, val * 24 as hour\n"
        "                       FROM (SELECT val - CAST(val as INT) as val, year, month, val as day\n"
        "                               FROM (SELECT val - CAST(val / 30.520183333 as INT) * 30.520183333 as val, year, val / 30.520183333 as month\n"
        "                                       FROM (SELECT val - CAST(val / 366.2422 as INT)  * 366.2422 as val, val / 366.2422  as year\n"
        "                                               FROM (SELECT sum((IFNULL(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333 as val FROM Series)))))))\n";
}
