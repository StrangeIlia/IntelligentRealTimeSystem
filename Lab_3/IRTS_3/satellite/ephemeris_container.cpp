#include "ephemeris_container.h"

EphemerisContainer::EphemerisContainer(bool memoryControl) : _memoryControl(memoryControl) {

}

EphemerisContainer::~EphemerisContainer() {
    if(_memoryControl) {
        for(auto inOneDay : _ephemeris) {
            for(auto inTime : inOneDay) {
                delete inTime;
            }
        }
    }
}

bool EphemerisContainer::memoryControlEnabled() const {
    return _memoryControl;
}

void EphemerisContainer::setMemoryControl(bool value) {
    _memoryControl = value;
}

bool EphemerisContainer::addEphemeris(Ephemeris *ephemeris) {
    if(!_ephemeris.contains(ephemeris->dateTime.date())) {
        _ephemeris.insert(ephemeris->dateTime.date(), EphemerisInOneDay());
    }
    auto &ephemerisInOneDay = _ephemeris[ephemeris->dateTime.date()];
    if(!ephemerisInOneDay.contains(ephemeris->dateTime.time())) {
        ephemerisInOneDay.insert(ephemeris->dateTime.time(), ephemeris);
        return true;
    } else {
        auto &ephemerisInTime = ephemerisInOneDay[ephemeris->dateTime.time()];
        for(
            auto iter = ephemeris->satelliteInfo.begin();
            iter != ephemeris->satelliteInfo.end();
            ++iter) {
            ephemerisInTime->satelliteInfo.insert(iter.key(), iter.value());
        }
        return false;
    }
}

void EphemerisContainer::removeEphemeris(Ephemeris *ephemeris) {
    if(!_ephemeris.contains(ephemeris->dateTime.date())) return;
    auto &ephemerisInOneDay = _ephemeris[ephemeris->dateTime.date()];
    if(!ephemerisInOneDay.contains(ephemeris->dateTime.time())) return;
    auto &ephemerisInTime = ephemerisInOneDay[ephemeris->dateTime.time()];
    for(
        auto iter = ephemeris->satelliteInfo.begin();
        iter != ephemeris->satelliteInfo.end();
        ++iter) {
        ephemerisInTime->satelliteInfo.remove(iter.key());
    }
    if(ephemerisInTime->satelliteInfo.empty())   ephemerisInOneDay.remove(ephemeris->dateTime.time());
    if(ephemerisInOneDay.empty())               _ephemeris.remove(ephemeris->dateTime.date());
}

EphemerisContainer::AllEphemeris EphemerisContainer::allEphemeris() const {
    return _ephemeris;
}

bool EphemerisContainer::containsDate(QDate date) const {
    return _ephemeris.contains(date);
}

bool EphemerisContainer::containeDateTime(QDateTime datetime) const {
    if(!_ephemeris.contains(datetime.date())) return false;
    return _ephemeris[datetime.date()].contains(datetime.time());
}

int EphemerisContainer::ephemerisCount() const {
    int count = 0;
    for(auto inOneDay : _ephemeris) {
        count += inOneDay.size();
    }
    return count;
}

Ephemeris *EphemerisContainer::ephemeris(QDateTime tpc) const {
    if(!_ephemeris.contains(tpc.date())) return nullptr;
    auto &ephemerisInOneDay = _ephemeris[tpc.date()];
    if(!ephemerisInOneDay.contains(tpc.time())) return nullptr;
    return ephemerisInOneDay[tpc.time()];;
}

Ephemeris *EphemerisContainer::nearbyEphemeris(QDateTime tpc) const {
    if(!_ephemeris.contains(tpc.date())) return nullptr;
    auto &ephemerisInOneDay = _ephemeris[tpc.date()];
    auto greaterTime = ephemerisInOneDay.lowerBound(tpc.time());
    auto lowerTime = greaterTime == ephemerisInOneDay.begin() ? greaterTime : greaterTime - 1;
    auto bestVariant = lowerTime;
    int diff = std::abs(tpc.time().secsTo(bestVariant.key()));
    ++greaterTime;
    while(++lowerTime != greaterTime) {
        int value = std::abs(tpc.time().secsTo(lowerTime.key()));
        if(value < diff) {
            diff = value;
            bestVariant = lowerTime;
        }
    }
    return bestVariant.value();
}

EphemerisContainer::DateList EphemerisContainer::getAllDates() const {
    return _ephemeris.keys();
}

EphemerisContainer::TimeList EphemerisContainer::getAllTimes(QDate date) const {
    if(!_ephemeris.contains(date)) return TimeList();
    return _ephemeris[date].keys();
}
