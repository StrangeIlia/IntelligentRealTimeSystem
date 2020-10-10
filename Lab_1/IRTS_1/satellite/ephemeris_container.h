#ifndef EPHEMERISCONTAINER_H
#define EPHEMERISCONTAINER_H

#include <QMap>
#include <QHash>
#include <QTime>
#include <QDate>
#include <QList>
#include <QDateTime>

#include "ephemeris.h"

class EphemerisContainer
{
public:
    typedef QMap<QTime, Ephemeris*>                 EphemerisInOneDay;
    typedef QHash<QDate, EphemerisInOneDay>         AllEphemeris;
    typedef QList<QDate>                            DateList;
    typedef QList<QTime>                            TimeList;
public:
    EphemerisContainer(bool memoryControl = true);
    ~EphemerisContainer();

    bool memoryControlEnabled() const;
    void setMemoryControl(bool value);

    bool addEphemeris(Ephemeris *ephemeris);
    void removeEphemeris(Ephemeris *ephemeris);
    AllEphemeris allEphemeris() const;

    bool containsDate(QDate date) const;
    bool containeDateTime(QDateTime datetime) const;

    int ephemerisCount() const;

    /// Возвращает сообщение
    /// Если такого сообщения нет, вернет nullptr
    Ephemeris *ephemeris(QDateTime tpc) const;
    /// Возвращает ближайшее сообщение, полученное в тот же день
    /// Если в этот день не было сообщений, вернет nullptr
    Ephemeris *nearbyEphemeris(QDateTime tpc) const;

    DateList getAllDates() const;
    TimeList getAllTimes(QDate date) const;

private:
    /// Ключ - номер спутника
    /// Значение - все сообщения для данного спутника
    AllEphemeris _ephemeris;
    bool _memoryControl = true;
};

#endif // EPHEMERISCONTAINER_H
