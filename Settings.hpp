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

#ifndef SETTINGSINFO_HPP
#define SETTINGSINFO_HPP

#include <string>
#include <vector>
#include <exception>
#include <wx/xml/xml.h>
#include "AppIDs.hpp"

struct Settings
{
    Settings();
    Settings(const wxString& fn); //load from xml file
    void Save(const wxString& fn);
    void InitCellColors(Settings& settings);
    void InitCellColors(std::vector<long>& colColors, int col);
    std::string defaultDb;
    bool defaultDbAsk = true;
    bool sortingByPronunciation = false;
    std::vector<long> colSizes;
    std::vector<std::vector<long>> cellColors;

    //for numerical based coloring
    enum{
        TEXT,
        BLANK,
        MIN,
        MID,
        MAX
    };
};

class SettingsException : public std::exception
{
public:
    virtual ~SettingsException() = default;

protected:
    virtual void AbstractDummy() = 0;
};

class SettingsLoadException : public SettingsException
{
public:
    virtual const char* what() const noexcept override {return "Invalid settings file.";}

protected:
    void AbstractDummy() override {}
};

class SettingsSaveException : public SettingsException
{
public:
    virtual const char* what() const noexcept override {return "Could not save settings file.";}

protected:
    void AbstractDummy() override {}
};

#endif
