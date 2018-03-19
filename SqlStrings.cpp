#include "SqlStrings.hpp"

namespace SqlStrings{
    const std::string medianStr =
        "select median({0})\n"
        "from Series\n";

    const std::string avgStr =
        "SELECT printf(\"%.2f\", AVG({0}))\n"
        "FROM Series\n";

    const std::string countWatchedStatus =
        "select count(Series.idWatchedStatus)\n"
        "from WatchedStatus\n"
        "left join Series\n"
        "on WatchedStatus.idWatchedStatus = Series.idWatchedStatus\n"
        "where WatchedStatus.idWatchedStatus <> 0\n"
        "group by WatchedStatus.idWatchedStatus\n"
        "order by WatchedStatus.idWatchedStatus\n";

    const std::string countReleaseType =
        "select count(Series.idReleaseType)\n"
        "from ReleaseType\n"
        "left join Series\n"
        "on ReleaseType.idReleaseType = Series.idReleaseType\n"
        "where ReleaseType.idReleaseType <> 0\n"
        "group by ReleaseType.idReleaseType\n"
        "order by ReleaseType.idReleaseType\n";

    const std::string timeWatchedSql =
        "SELECT *\n"
        "FROM\n"
        "(\n"
        "       SELECT printf(\"%.2f\",  sum(episodesWatched * episodeLength) / 1436.068333333) , 0 as idx FROM Series\n\n"

        "       UNION ALL\n\n"

        "       --There are 366.2422 stellar days in a year\n"
        "       --Dividing by 12 gives us 30.520183333 days in a month\n"
        "       --A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "       SELECT printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  year, month, day, hour, minute ) , 1 as idx\n"
        "       FROM(SELECT year, month, day, hour, val * 60 as minute\n"
        "               FROM(SELECT val * 24 - CAST(val * 24 as INT) as val, year, month, day, val * 24 as hour\n"
        "                       FROM (SELECT val - CAST(val as INT) as val, year, month, val as day\n"
        "                               FROM (SELECT val - CAST(val / 30.520183333 as INT) * 30.520183333 as val, year, val / 30.520183333 as month\n"
        "                                       FROM (SELECT val - CAST(val / 366.2422 as INT)  * 366.2422 as val, val / 366.2422  as year\n"
        "                                               FROM (SELECT sum(episodesWatched * episodeLength) / 1436.068333333 as val FROM Series))))))\n"
        ")\n"
        "ORDER BY idx asc";

    const std::string timeRewatchedSql =
        "SELECT *\n"
        "FROM\n"
        "(\n"
        "       SELECT printf(\"%.2f\",  sum((IFNULL(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333) , 0 as idx FROM Series\n\n"

        "       UNION ALL\n\n"

        "       --There are 366.2422 stellar days in a year\n"
        "       --Dividing by 12 gives us 30.520183333 days in a month\n"
        "       --A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "       SELECT printf(\"%d Years, %02d Months, %02d Days, %02d Hours, %02d Minutes\",  year, month, day, hour, minute ) , 1 as idx\n"
        "       FROM(SELECT year, month, day, hour, val * 60 as minute\n"
        "               FROM(SELECT val * 24 - CAST(val * 24 as INT) as val, year, month, day, val * 24 as hour\n"
        "                       FROM (SELECT val - CAST(val as INT) as val, year, month, val as day\n"
        "                               FROM (SELECT val - CAST(val / 30.520183333 as INT) * 30.520183333 as val, year, val / 30.520183333 as month\n"
        "                                       FROM (SELECT val - CAST(val / 366.2422 as INT)  * 366.2422 as val, val / 366.2422  as year\n"
        "                                               FROM (SELECT sum((IFNULL(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333 as val FROM Series))))))\n"
        ")\n"
        "ORDER BY idx asc";

}
