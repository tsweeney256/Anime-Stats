PRAGMA foreign_keys=off;

UPDATE `Version` set `version` = 3;

ALTER TABLE `SavedFilterSort` RENAME TO `SavedFilterSort_old`;

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
        "SavedFilterSortName" ( "idSavedFilterSortName" )
        ON DELETE CASCADE ON UPDATE CASCADE
);

INSERT INTO `SavedFilterSort` SELECT * FROM `SavedFilterSort_old`;
DROP TABLE `SavedFilterSort_old`;

PRAGMA foreign_keys=on;
