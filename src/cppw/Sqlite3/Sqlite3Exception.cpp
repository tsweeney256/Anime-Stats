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

#include "../Sqlite3.hpp"

cppw::Sqlite3Exception::Sqlite3Exception(
    sqlite3* connection, int errCode)
    : m_connection(connection), m_errCode(errCode) {}

int cppw::Sqlite3Exception::GetErrorCode() const
{
    return m_errCode;
}

const char* cppw::Sqlite3Exception::what() const noexcept
{
    return sqlite3_errmsg(m_connection);
}
