#ifndef SQLSTRINGS_HPP
#define SQLSTRING_HPP

namespace SqlStrings{
    /*SELECT AVG(%s)
    FROM (SELECT %s
          FROM Series
          where %s is not null
          ORDER BY %s
          LIMIT 2 - (SELECT COUNT(%s) FROM Series) % 2
          OFFSET (SELECT (COUNT(%s) - 1) / 2
                  FROM Series))*/
    const char* medianStr = "SELECT AVG(%s) FROM (SELECT %s FROM Series where %s is not null ORDER BY %s LIMIT 2 - (SELECT COUNT(%s) FROM Series) % 2 OFFSET (SELECT (COUNT(%s) - 1) / 2 FROM Series))";
    const int numToFormatMedianStr = 6;
}

#endif
