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

#include "AppIDs.hpp"

bool col::isColText(int col)
{
    return col == TITLE || col == PRONUNCIATION || col == DATE_FINISHED || col == DATE_STARTED;
}

bool col::isColNumeric(int col)
{
    return col == RATING || col == YEAR || col == EPISODES_WATCHED || col == TOTAL_EPISODES ||
        col == REWATCHED_EPISODES || col == EPISODE_LENGTH;
}

bool col::isColLimitedValue(int col)
{
    return col == WATCHED_STATUS || col == RELEASE_TYPE || col == SEASON;
}
