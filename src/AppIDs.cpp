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
