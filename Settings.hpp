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
    std::string defaultDb;
    bool sortingByPronunciation = false;
    std::vector<int> colSizes;
    std::vector<std::vector<int>> cellColors;

    //for numerical based coloring
    enum{
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
