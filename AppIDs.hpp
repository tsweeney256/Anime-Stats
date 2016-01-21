#ifndef APPIDS_HPP
#define APPIDS_HPP

#include <vector>
#include <wx/defs.h>

//the various custom IDs that we use for the application
enum
{
    //start from 1 above the highest ID that wxWidgets can make itself
    //this way we ensure we never collide with whatever IDs the library itself makes
    ID_DATA_GRID = wxID_HIGHEST + 1,
    ID_NOTEBOOK,
    ID_WATCHED_CB,
    ID_WATCHING_CB,
    ID_STALLED_CB,
    ID_DROPPED_CB,
    ID_BLANK_CB,
    ID_ALL_CB,
    ID_APPLY_FILTER_BTN,
	ID_RESET_FILTER_BTN,
	ID_ADV_FILTER_BTN,
	ID_REFRESH_BTN,
	ID_ADD_ROW_BTN,
	ID_DELETE_ROW_BTN,
    ID_TITLE_FILTER_FIELD,
    ID_ADV_FILTER_FRAME,
    ID_ADV_FILTER_WATCHED_STATUS,
    ID_ADV_FILTER_WATCHED_STATUS_ALL,
    ID_ADV_FILTER_RELEASE_TYPE,
    ID_ADV_FILTER_RELEASE_TYPE_ALL,
    ID_ADV_FILTER_SEASON,
    ID_ADV_FILTER_SEASON_ALL,
    ID_ADV_FILTER_OK,
    ID_ADV_FILTER_CANCEL,
    ID_ADV_FILTER_APPLY,
    ID_ADV_FILTER_RESET
};

namespace col
{
    enum
    {
        ID_SERIES = 0,
        TITLE,
        RATING,
        WATCHED_STATUS,
        RELEASE_TYPE,
        YEAR,
        SEASON,
        EPISODES_WATCHED,
        TOTAL_EPISODES,
        REWATCHED_EPISODES,
        EPISODE_LENGTH,
        DATE_STARTED,
        DATE_FINISHED
    };
}

const int numTitleCols = 4;
const int numSeriesCols = 12;
const int numViewCols = 13; //including the hidden 1
const std::vector<std::string> colViewName {"idSeries", "name", "rating", "idWatchedStatus", "idReleaseType", "year", "idSeason",
    "episodesWatched", "totalEpisodes", "rewatchedEpisodes", "episodeLength", "dateStarted", "dateFinished"};

#endif // APPIDS_HPP
