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

#include <string>
#include "BasicSelect.hpp"

namespace SqlStrings
{
    const std::string basicSelect =
        "select distinct rightSide.idSeries, name as Title, pronunciation as Pronunciation, rating as Rating, status as \"Watched Status\",\n"
        "    type as \"Release Type\", year as Year, season as Season, episodesWatched as \"Episodes Watched\",\n"
        "    totalEpisodes as \"Total Episodes\", rewatchedEpisodes as \"Rewatched Episodes\" , episodeLength as \"Episode Length\",\n"
        "    dateStarted as \"Date Started\", dateFinished as \"Date Finished\", nameSort,\n"
        "    idWatchedStatus, idReleaseType, episodesWatched, rewatchedEpisodes, episodeLength, rating, 1 as constant {tag_cols}\n"
        "from (select distinct LeftTitle.idSeries\n"
        "from (select * from Title inner join Label on Title.idLabel = Label.idLabel) as LeftTitle\n"
        "left join (select name, idSeries from Title inner join Label on Title.idLabel = Label.idLabel) as RightTitle\n"
        "on LeftTitle.idSeries = RightTitle.idSeries\n"
        "where LeftTitle.name like ?1 or RightTitle.name like ?1) as LeftSide\n"
        "inner join\n"
        "(\n"
        "    select *\n"
        "    from (select * from Title inner join Label on Title.idLabel = Label.idLabel where Main = 1) as pTitle\n"
        "    left join Series\n"
        "    on pTitle.idSeries = series.idSeries\n"
        "    left join ReleaseType\n"
        "    on series.idReleaseType = ReleaseType.idReleaseType\n"
        "    left join Season\n"
        "    on series.idSeason = Season.idSeason\n"
        "    left join WatchedStatus\n"
        "    on series.idWatchedStatus = WatchedStatus.idWatchedStatus\n"
        "    left join Tag\n"
        "    on series.idSeries = Tag.idSeries\n"
        "    inner join\n"
        "    (\n"
        "        SELECT idName, nameSort\n"
        "        FROM\n"
        "        (\n"
        "            SELECT idName, name AS nameSort\n"
        "            FROM Title\n"
        "            WHERE pronunciation IS NULL\n"
        "\n"
        "            UNION\n"
        "\n"
        "            SELECT idName, pronunciation AS nameSort\n"
        "            FROM Title\n"
        "            WHERE pronunciation IS NOT NULL\n"
        "        )\n"
        "    ) as NameSort\n"
        "    on pTitle.idName = NameSort.idName\n"
        ") as RightSide\n"
        "on LeftSide.idSeries = rightSide.idSeries\n";
}
