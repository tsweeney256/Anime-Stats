#ifndef FILTERSTRUCTS_HPP
#define FILTERSTRUCTS_HPP

#include <memory>

struct BasicFilterInfo
{
    static std::unique_ptr<BasicFilterInfo> MakeUnique()
        { return std::unique_ptr<BasicFilterInfo>(new BasicFilterInfo()); }
    static std::shared_ptr<BasicFilterInfo> MakeShared()
        { return std::shared_ptr<BasicFilterInfo>(new BasicFilterInfo()); }

    bool watched = true;
    bool watching = true;
    bool stalled = true;
    bool dropped = true;
    bool watchedBlank = true;
    std::string title;

private:
    BasicFilterInfo() {}
};

struct AdvFilterInfo
{
    static std::unique_ptr<AdvFilterInfo> MakeUnique()
        { return std::unique_ptr<AdvFilterInfo>(new AdvFilterInfo()); }
    static std::shared_ptr<AdvFilterInfo> MakeShared()
        { return std::shared_ptr<AdvFilterInfo>(new AdvFilterInfo()); }

    bool tv = true;
    bool movie = true;
    bool ova = true;
    bool ona = true;
    bool tvSpecial = true;
    bool releaseBlank = true;

    bool winter = true;
    bool spring = true;
    bool summer = true;
    bool fall = true;
    bool seasonBlank = true;

    bool ratingEnabled = false;
    bool yearEnabled = false;
    bool epsWatchedEnabled = false;
    bool epsRewatchedEnabled = false;
    bool totalEpsEnabled = false;
    bool lengthEnabled = false;
    bool dateStartedEnabled = false;
    bool dateFinishedEnabled = false;

    int ratingLow = 0;
    int ratingHigh = 0;
    int yearLow = 0;
    int yearHigh = 0;
    int epsWatchedLow = 0;
    int epsWatchedHigh = 0;
    int epsRewatchedLow = 0;
    int epsRewatchedHigh = 0;
    int totalEpsLow = 0;
    int totalEpsHigh = 0;
    int lengthLow = 0;
    int lengthHigh = 0;

    int yearStartedLow = 0;
    int yearStartedHigh = 0;
    int monthStartedLow = 1;
    int monthStartedHigh = 1;
    int dayStartedLow = 1;
    int dayStartedHigh = 1;

    int yearFinishedLow = 0;
    int yearFinishedHigh = 0;
    int monthFinishedLow = 1;
    int monthFinishedHigh = 1;
    int dayFinishedLow = 1;
    int dayFinishedHigh = 1;

private:
    AdvFilterInfo() {}
};

#endif
