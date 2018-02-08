/*Anime Stats
Copyright (C) 2018 Thomas Sweeney
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

CREATE TABLE "Version" (
    `version` INTEGER NOT NULL
);
INSERT INTO `Version` VALUES (1);

CREATE TABLE "SavedFilter" (
    `idSavedFilter` INTEGER,
    `name` TEXT NOT NULL,
    `default` INTEGER NOT NULL,
    PRIMARY KEY(`idSavedFilter`),
    UNIQUE (`name`)
);
INSERT INTO `SavedFilter` VALUES (0, 'All', 1);
INSERT INTO `SavedFilter` VALUES (1, 'Watched', 0);
INSERT INTO `SavedFilter` VALUES (2, 'Watching', 0);
INSERT INTO `SavedFilter` VALUES (3, 'Stalled', 0);
INSERT INTO `SavedFilter` VALUES (4, 'Dropped', 0);
INSERT INTO `SavedFilter` VALUES (5, 'To Watch', 0);
INSERT INTO `SavedFilter` VALUES (6, 'Blank', 0);
CREATE TABLE "SavedFilterArgName" (
    `idSavedFilterArgName` INTEGER,
    `name` TEXT,
    PRIMARY KEY(`idSavedFilterArgName`),
    UNIQUE (`name`)
);
INSERT INTO `SavedFilterArgName` VALUES (0, 'watched');
INSERT INTO `SavedFilterArgName` VALUES (1, 'watching');
INSERT INTO `SavedFilterArgName` VALUES (2, 'stalled');
INSERT INTO `SavedFilterArgName` VALUES (3, 'dropped');
INSERT INTO `SavedFilterArgName` VALUES (4, 'toWatch');
INSERT INTO `SavedFilterArgName` VALUES (5, 'blank');
INSERT INTO `SavedFilterArgName` VALUES (6, 'tv');
INSERT INTO `SavedFilterArgName` VALUES (7, 'movie');
INSERT INTO `SavedFilterArgName` VALUES (8, 'ova');
INSERT INTO `SavedFilterArgName` VALUES (9, 'ona');
INSERT INTO `SavedFilterArgName` VALUES (10, 'tvSpecial');
INSERT INTO `SavedFilterArgName` VALUES (11, 'releaseBlank');
INSERT INTO `SavedFilterArgName` VALUES (12, 'winter');
INSERT INTO `SavedFilterArgName` VALUES (13, 'spring');
INSERT INTO `SavedFilterArgName` VALUES (14, 'summer');
INSERT INTO `SavedFilterArgName` VALUES (15, 'fall');
INSERT INTO `SavedFilterArgName` VALUES (16, 'seasonBlank');
INSERT INTO `SavedFilterArgName` VALUES (17, 'ratingEnabled');
INSERT INTO `SavedFilterArgName` VALUES (18, 'yearEnabled');
INSERT INTO `SavedFilterArgName` VALUES (19, 'epsWatchedEnabled');
INSERT INTO `SavedFilterArgName` VALUES (20, 'epsRewatchedEnabled');
INSERT INTO `SavedFilterArgName` VALUES (21, 'totalEpsEnabled');
INSERT INTO `SavedFilterArgName` VALUES (22, 'lengthEnabled');
INSERT INTO `SavedFilterArgName` VALUES (23, 'dateStartedEnabled');
INSERT INTO `SavedFilterArgName` VALUES (24, 'dateFinishedEnabled');
INSERT INTO `SavedFilterArgName` VALUES (25, 'ratingLow');
INSERT INTO `SavedFilterArgName` VALUES (26, 'ratingHigh');
INSERT INTO `SavedFilterArgName` VALUES (27, 'yearLow');
INSERT INTO `SavedFilterArgName` VALUES (28, 'yearHigh');
INSERT INTO `SavedFilterArgName` VALUES (29, 'epsWatchedLow');
INSERT INTO `SavedFilterArgName` VALUES (30, 'epsWatchedHigh');
INSERT INTO `SavedFilterArgName` VALUES (31, 'epsRewatchedLow');
INSERT INTO `SavedFilterArgName` VALUES (32, 'epsRewatchedHigh');
INSERT INTO `SavedFilterArgName` VALUES (33, 'totalEpsLow');
INSERT INTO `SavedFilterArgName` VALUES (34, 'totalEpsHigh');
INSERT INTO `SavedFilterArgName` VALUES (35, 'lengthLow');
INSERT INTO `SavedFilterArgName` VALUES (36, 'lengthHigh');
INSERT INTO `SavedFilterArgName` VALUES (37, 'yearStartedLow');
INSERT INTO `SavedFilterArgName` VALUES (38, 'yearStartedHigh');
INSERT INTO `SavedFilterArgName` VALUES (39, 'monthStartedLow');
INSERT INTO `SavedFilterArgName` VALUES (40, 'monthStartedHigh');
INSERT INTO `SavedFilterArgName` VALUES (41, 'dayStartedLow');
INSERT INTO `SavedFilterArgName` VALUES (42, 'dayStartedHigh');
INSERT INTO `SavedFilterArgName` VALUES (43, 'yearFinishedLow');
INSERT INTO `SavedFilterArgName` VALUES (44, 'yearFinishedHigh');
INSERT INTO `SavedFilterArgName` VALUES (45, 'monthFinishedLow');
INSERT INTO `SavedFilterArgName` VALUES (46, 'monthFinishedHigh');
INSERT INTO `SavedFilterArgName` VALUES (47, 'dayFinishedLow');
INSERT INTO `SavedFilterArgName` VALUES (48, 'dayFinishedHigh');
CREATE TABLE "SavedFilterArg" (
    `idSavedFilterArg` INTEGER,
    `idSavedFilter` INTEGER NOT NULL,
    `idSavedFilterArgName` INTEGER NOT NULL,
    `value` INTEGER NOT NULL,
    PRIMARY KEY (`idSavedFilterArg`),
    FOREIGN KEY (`idSavedFilter`) REFERENCES "SavedFilter" ( "idSavedFilter" )
        ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (`idSavedFilterArgName`)
        REFERENCES "SavedFilterArgName" ( "idSavedFilterArgName" )
        ON DELETE CASCADE ON UPDATE CASCADE,
    UNIQUE (`idSavedFilter`, `idSavedFilterArgName`)
);
INSERT INTO `SavedFilterArg` VALUES (0, 1, 0, 1);
INSERT INTO `SavedFilterArg` VALUES (1, 1, 1, 0);
INSERT INTO `SavedFilterArg` VALUES (2, 1, 2, 0);
INSERT INTO `SavedFilterArg` VALUES (3, 1, 3, 0);
INSERT INTO `SavedFilterArg` VALUES (4, 1, 4, 0);
INSERT INTO `SavedFilterArg` VALUES (5, 1, 5, 0);
INSERT INTO `SavedFilterArg` VALUES (6, 2, 0, 0);
INSERT INTO `SavedFilterArg` VALUES (7, 2, 1, 1);
INSERT INTO `SavedFilterArg` VALUES (8, 2, 2, 0);
INSERT INTO `SavedFilterArg` VALUES (9, 2, 3, 0);
INSERT INTO `SavedFilterArg` VALUES (10, 2, 4, 0);
INSERT INTO `SavedFilterArg` VALUES (11, 2, 5, 0);
INSERT INTO `SavedFilterArg` VALUES (12, 3, 0, 0);
INSERT INTO `SavedFilterArg` VALUES (13, 3, 1, 0);
INSERT INTO `SavedFilterArg` VALUES (14, 3, 2, 1);
INSERT INTO `SavedFilterArg` VALUES (15, 3, 3, 0);
INSERT INTO `SavedFilterArg` VALUES (16, 3, 4, 0);
INSERT INTO `SavedFilterArg` VALUES (17, 3, 5, 0);
INSERT INTO `SavedFilterArg` VALUES (18, 4, 0, 0);
INSERT INTO `SavedFilterArg` VALUES (19, 4, 1, 0);
INSERT INTO `SavedFilterArg` VALUES (20, 4, 2, 0);
INSERT INTO `SavedFilterArg` VALUES (21, 4, 3, 1);
INSERT INTO `SavedFilterArg` VALUES (22, 4, 4, 0);
INSERT INTO `SavedFilterArg` VALUES (23, 4, 5, 0);
INSERT INTO `SavedFilterArg` VALUES (24, 5, 0, 0);
INSERT INTO `SavedFilterArg` VALUES (25, 5, 1, 0);
INSERT INTO `SavedFilterArg` VALUES (26, 5, 2, 0);
INSERT INTO `SavedFilterArg` VALUES (27, 5, 3, 0);
INSERT INTO `SavedFilterArg` VALUES (28, 5, 4, 1);
INSERT INTO `SavedFilterArg` VALUES (29, 5, 5, 0);
INSERT INTO `SavedFilterArg` VALUES (30, 6, 0, 0);
INSERT INTO `SavedFilterArg` VALUES (31, 6, 1, 0);
INSERT INTO `SavedFilterArg` VALUES (32, 6, 2, 0);
INSERT INTO `SavedFilterArg` VALUES (33, 6, 3, 0);
INSERT INTO `SavedFilterArg` VALUES (34, 6, 4, 0);
INSERT INTO `SavedFilterArg` VALUES (35, 6, 5, 1);
