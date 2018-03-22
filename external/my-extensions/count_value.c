/*Copyright (C) 2018 Thomas Sweeney
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

void count_value_step(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
    int* num_occurences = sqlite3_aggregate_context(ctx, sizeof(int));
    int size1 = sqlite3_value_bytes(argv[0]);
    int size2 = sqlite3_value_bytes(argv[1]);
    const char* val1 = sqlite3_value_blob(argv[0]);
    const char* val2 = sqlite3_value_blob(argv[1]);

    if (size1 != size2){
        return;
    }
    for (int i = 0; i < size1; ++i) {
        if (val1[i] != val2[i]) {
            return;
        }
    }
    (*num_occurences)++;
}

void count_value_finalize(sqlite3_context* ctx)
{
    int* num_occurences = sqlite3_aggregate_context(ctx, sizeof(int));
    sqlite3_result_int(ctx, *num_occurences);
}

int sqlite3_countvalue_init(
    sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  sqlite3_create_function(db, "count", 2, SQLITE_UTF8, 0, 0,
                          count_value_step, count_value_finalize);
  return rc;
}
