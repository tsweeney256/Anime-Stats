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

select rightSide.idSeries, name as Title, pronunciation as Pronunciation, rating as Rating, status as "Watched Status", 
	type as "Release Type", year as Year, season as Season, episodesWatched as "Episodes Watched", totalEpisodes as "Total Episodes", 
	rewatchedEpisodes as "Rewatched Episodes" , episodeLength as "Episode Length", dateStarted as "Date Started", 
	dateFinished as "Date Finished"
from (select distinct LeftTitle.idSeries
from (select * from Title inner join Label on Title.idLabel = Label.idLabel) as LeftTitle
left join (select name, idSeries from Title inner join Label on Title.idLabel = Label.idLabel) as RightTitle
on LeftTitle.idSeries = RightTitle.idSeries
where LeftTitle.name like ? or RightTitle.name like ?) as LeftSide
inner join 
(select *
from (select * from Title inner join Label on Title.idLabel = Label.idLabel where Main = 1) as pTitle
left join Series
on pTitle.idSeries = series.idSeries
left join ReleaseType
on  series.idReleaseType = ReleaseType.idReleaseType
left join Season
on series.idSeason = Season.idSeason
left join WatchedStatus
on series.idWatchedStatus = WatchedStatus.idWatchedStatus) as RightSide
on LeftSide.idSeries = rightSide.idSeries



