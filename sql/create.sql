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

BEGIN TRANSACTION;
CREATE TABLE "WatchedStatus" (
  "idWatchedStatus" INTEGER PRIMARY KEY,
  "status" TEXT NOT NULL
);
INSERT INTO `WatchedStatus` VALUES (0,'');
INSERT INTO `WatchedStatus` VALUES (1,'Watched');
INSERT INTO `WatchedStatus` VALUES (2,'Watching');
INSERT INTO `WatchedStatus` VALUES (3,'Stalled');
INSERT INTO `WatchedStatus` VALUES (4,'Dropped');
CREATE TABLE "Title" (
  "idName" INTEGER PRIMARY KEY,
  "name" TEXT NOT NULL,
  "idSeries" INTEGER NOT NULL,
  "idLabel" INTEGER NOT NULL DEFAULT 0,
  CONSTRAINT "fk_Title_Show1" FOREIGN KEY ("idSeries") REFERENCES "Series" ("idSeries") ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT "fk_Title_Label1" FOREIGN KEY ("idLabel") REFERENCES "Label" ("idLabel") ON DELETE SET DEFAULT ON UPDATE CASCADE
);
CREATE TABLE "Series" (
  "idSeries" INTEGER PRIMARY KEY,
  "rating" INTEGER DEFAULT NULL,
  "idReleaseType" INTEGER DEFAULT 0,
  "idWatchedStatus" INTEGER DEFAULT 0,
  "year" INTEGER DEFAULT NULL,
  "idSeason" INTEGER DEFAULT 0,
  "episodesWatched" INTEGER DEFAULT NULL,
  "totalEpisodes" INTEGER DEFAULT NULL,
  "rewatchedEpisodes" INTEGER DEFAULT NULL,
  "episodeLength" INTEGER DEFAULT NULL,
  "dateStarted" TEXT DEFAULT NULL,
  "dateFinished" TEXT DEFAULT NULL,
  CONSTRAINT "fk_Show_LengthType1" FOREIGN KEY ("idLengthType") REFERENCES "LengthType" ("idLengthType") ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT "fk_Show_ReleaseType1" FOREIGN KEY ("idReleaseType") REFERENCES "ReleaseType" ("idReleaseType") ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT "fk_Show_Season1" FOREIGN KEY ("idSeason") REFERENCES "Season" ("idSeason") ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT "fk_Show_WatchedStatus1" FOREIGN KEY ("idWatchedStatus") REFERENCES "WatchedStatus" ("idWatchedStatus") ON DELETE NO ACTION ON UPDATE NO ACTION
);
CREATE TABLE "Season" (
  "idSeason" INTEGER PRIMARY KEY,
  "season" TEXT DEFAULT NULL
);
INSERT INTO `Season` VALUES (0,'');
INSERT INTO `Season` VALUES (1,'Winter');
INSERT INTO `Season` VALUES (2,'Spring');
INSERT INTO `Season` VALUES (3,'Summer');
INSERT INTO `Season` VALUES (4,'Fall');
CREATE TABLE "ReleaseType" (
  "idReleaseType" INTEGER PRIMARY KEY,
  "type" TEXT NOT NULL
);
INSERT INTO `ReleaseType` VALUES (0,'');
INSERT INTO `ReleaseType` VALUES (1,'TV');
INSERT INTO `ReleaseType` VALUES (2,'OVA');
INSERT INTO `ReleaseType` VALUES (3,'ONA');
INSERT INTO `ReleaseType` VALUES (4,'Movie');
INSERT INTO `ReleaseType` VALUES (5,'TV Special');
CREATE TABLE "Label" (
	`idLabel`	INTEGER,
	`label`	TEXT NOT NULL,
	`Main`	INTEGER DEFAULT 0,
	PRIMARY KEY(idLabel)
);
INSERT INTO `Label` VALUES (0,'No Label',0);
INSERT INTO `Label` VALUES (1,'Default',1);
COMMIT;
