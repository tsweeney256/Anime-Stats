UPDATE `Version` set `version` = 2;

CREATE TABLE "SavedFilterSortName" (
    `idSavedFilterSortName` INTEGER,
    `name` TEXT NOT NULL,
    PRIMARY KEY (`idSavedFilterSortName`),
    UNIQUE (`name`)
);
INSERT INTO `SavedFilterSortName` (`idSavedFilterSortName`, `name`) VALUES
    (1, 'nameSort'),
    (2, 'Rating'),
    (3, 'Watched Status'),
    (4, 'Release Type'),
    (5, 'Year'),
    (6, 'Season'),
    (7, 'Episodes Watched'),
    (8, 'Total Episodes'),
    (9, 'Rewatched Episodes'),
    (10, 'Episode Length'),
    (11, 'Date Started'),
    (12, 'Date Finished');

CREATE TABLE "SavedFilterSort" (
    `idSavedFilterSort` INTEGER,
    `idSavedFilter` INTEGER NOT NULL,
    `idSavedFilterSortName` INTEGER NOT NULL,
    `asc` INTEGER NOT NULL,
    PRIMARY KEY (`idSavedFilterSort`),
    UNIQUE (`idSavedFilter`, `idSavedFilterSortName`),
    FOREIGN KEY (`idSavedFilter`) REFERENCES "SavedFilter" ( "idSavedFilter" )
        ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (`idSavedFilterSortName`) REFERENCES
        "SavedFilter" ( "idSavedFilter" ) ON DELETE CASCADE ON UPDATE CASCADE
);
