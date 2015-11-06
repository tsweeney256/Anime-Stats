BEGIN TRANSACTION;
CREATE TABLE "WatchedStatus" (
  "idWatchedStatus" INTEGER PRIMARY KEY,
  "status" TEXT NOT NULL
);
INSERT INTO `WatchedStatus` VALUES (0,'Watched');
INSERT INTO `WatchedStatus` VALUES (1,'Watching');
INSERT INTO `WatchedStatus` VALUES (2,'Stalled');
INSERT INTO `WatchedStatus` VALUES (3,'Dropped');
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
  "idReleaseType" INTEGER DEFAULT NULL,
  "idWatchedStatus" INTEGER DEFAULT NULL,
  "idLengthType" INTEGER DEFAULT NULL,
  "year" INTEGER DEFAULT NULL,
  "idSeason" INTEGER DEFAULT NULL,
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
INSERT INTO `Season` VALUES (0,'Winter');
INSERT INTO `Season` VALUES (1,'Spring');
INSERT INTO `Season` VALUES (2,'Summer');
INSERT INTO `Season` VALUES (3,'Fall');
CREATE TABLE "ReleaseType" (
  "idReleaseType" INTEGER PRIMARY KEY,
  "type" TEXT NOT NULL
);
INSERT INTO `ReleaseType` VALUES (0,'TV');
INSERT INTO `ReleaseType` VALUES (1,'OVA');
INSERT INTO `ReleaseType` VALUES (2,'ONA');
INSERT INTO `ReleaseType` VALUES (3,'Movie');
INSERT INTO `ReleaseType` VALUES (4,'TV Special');
CREATE TABLE "LengthType" (
  "idLengthType" INTEGER PRIMARY KEY,
  "type" TEXT NOT NULL
);
INSERT INTO `LengthType` VALUES (0,'Normal');
INSERT INTO `LengthType` VALUES (1,'Half');
INSERT INTO `LengthType` VALUES (2,'Short');
INSERT INTO `LengthType` VALUES (3,'Long');
CREATE TABLE "Label" (
  "idLabel" INTEGER PRIMARY KEY,
  "label" TEXT NOT NULL
);
INSERT INTO `Label` VALUES (0,'No Label');
INSERT INTO `Label` VALUES (1,'Default');
COMMIT;
