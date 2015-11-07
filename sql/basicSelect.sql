select series.idSeries, name as Title, rating as Rating, type as "Release Type", status as "Watched Status", year as Year, 
	season as Season, episodesWatched as "Episodes Watched", totalEpisodes as "Total Episodes", 
	rewatchedEpisodes as "Rewatched Episodes" , episodeLength as "Episode Length", dateStarted as "Date Started", dateFinished as "Date Finished"
from (select * from Title inner join Label on Title.idLabel = Label.idLabel where Main = 1) as pTitle
left join Series
on pTitle.idSeries = series.idSeries
left join ReleaseType
on  series.idReleaseType = ReleaseType.idReleaseType
left join Season
on series.idSeason = Season.idSeason
left join WatchedStatus
on series.idWatchedStatus = WatchedStatus.idWatchedStatus
