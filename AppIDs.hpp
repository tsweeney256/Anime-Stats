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

#ifndef APPIDS_HPP
#define APPIDS_HPP

#include <vector>
#include <wx/defs.h>

namespace col
{
    enum
    {
        ID_SERIES = 0,
        FIRST_VISIBLE_COL,
        TITLE = FIRST_VISIBLE_COL,
        PRONUNCIATION,
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
        DATE_FINISHED,
        NUM_COLS
    };
}

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
	ID_ADV_SORT_BTN,
	ID_REFRESH_BTN,
	ID_ADD_ROW_BTN,
	ID_DELETE_ROW_BTN,
    ID_TITLE_ALIAS_BTN,
    ID_TITLE_FILTER_FIELD,
    ID_ADV_FILTER_FRAME,
    ID_ADV_FILTER_WATCHED_STATUS,
    ID_ADV_FILTER_WATCHED_STATUS_ALL,
    ID_ADV_FILTER_RELEASE_TYPE,
    ID_ADV_FILTER_RELEASE_TYPE_ALL,
    ID_ADV_FILTER_SEASON,
    ID_ADV_FILTER_SEASON_ALL,
    ID_ADV_FILTER_RATING_ENABLE,
    ID_ADV_FILTER_YEAR_ENABLE,
    ID_ADV_FILTER_EPS_WATCHED_ENABLE,
    ID_ADV_FILTER_EPS_REWATCHED_ENABLE,
    ID_ADV_FILTER_TOTAL_EPS_ENABLE,
    ID_ADV_FILTER_LENGTH_ENABLE,
    ID_ADV_FILTER_DATE_STARTED_ENABLE,
    ID_ADV_FILTER_DATE_FINISHED_ENABLE,
    ID_ADV_FILTER_OK,
    ID_ADV_FILTER_CANCEL,
    ID_ADV_FILTER_APPLY,
    ID_ADV_FILTER_RESET,
    ID_ADV_SORT_FRAME,
    ID_VIEW_COL_BEGIN, //the very first column
    ID_VIEW_COL_END = ID_VIEW_COL_BEGIN + col::NUM_COLS - 1 //the last column
};

const int numTitleCols = 5;
const int numSeriesCols = 12;
const int numViewCols = 14; //including the hidden 1
const std::vector<std::string> colViewName {"idSeries", "name", "pronunciation", "rating", "idWatchedStatus", "idReleaseType", "year", "idSeason",
    "episodesWatched", "totalEpisodes", "rewatchedEpisodes", "episodeLength", "dateStarted", "dateFinished"};

#endif // APPIDS_HPP
