#include "SqlStrings.hpp"

namespace SqlStrings{
    const std::string medianStr =
        "SELECT AVG(%1$s)\n"
        "FROM\n"
        "    (SELECT %1$s\n"
        "     FROM Series\n"
        "     WHERE %1$s is not NULL\n"
        "     ORDER BY %1$s\n"
        "     LIMIT 2 - (SELECT COUNT(%1$s) FROM Series) % 2\n"
        "     OFFSET (SELECT (COUNT(%1$s) - 1) / 2\n"
        "     FROM Series))\n";

    const std::string timeWatchedSql =
        "SELECT *\n"
        "FROM\n"
        "(\n"
        "       SELECT printf(\"%.2f\",  sum(episodesWatched * episodeLength) / 1436.068333333) , 0 as idx FROM Series\n\n"

        "       UNION\n\n"

        "       --There are 366.2422 stellar days in a year\n"
        "       --Dividing by 12 gives us 30.520183333 days in a month\n"
        "       --A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "       SELECT printf(\"%d Years, %d Months, %d Days, %d Hours, %d Minutes\",  year, month, day, hour, minute ) , 1 as idx\n"
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

        "       UNION\n\n"

        "       --There are 366.2422 stellar days in a year\n"
        "       --Dividing by 12 gives us 30.520183333 days in a month\n"
        "       --A stellar day is 23 hours 56 minutes and 4.1 seconds long or 1436.068333333 minutes long\n"
        "       SELECT printf(\"%d Years, %d Months, %d Days, %d Hours, %d Minutes\",  year, month, day, hour, minute ) , 1 as idx\n"
        "       FROM(SELECT year, month, day, hour, val * 60 as minute\n"
        "               FROM(SELECT val * 24 - CAST(val * 24 as INT) as val, year, month, day, val * 24 as hour\n"
        "                       FROM (SELECT val - CAST(val as INT) as val, year, month, val as day\n"
        "                               FROM (SELECT val - CAST(val / 30.520183333 as INT) * 30.520183333 as val, year, val / 30.520183333 as month\n"
        "                                       FROM (SELECT val - CAST(val / 366.2422 as INT)  * 366.2422 as val, val / 366.2422  as year\n"
        "                                               FROM (SELECT sum((IFNULL(rewatchedEpisodes, 0) + episodesWatched) * episodeLength) / 1436.068333333 as val FROM Series))))))\n"
        ")\n"
        "ORDER BY idx asc";

}
