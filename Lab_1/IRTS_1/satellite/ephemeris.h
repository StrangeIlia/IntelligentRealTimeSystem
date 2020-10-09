#ifndef EPHEMERIS_H
#define EPHEMERIS_H

#include <QHash>
#include <QDateTime>

struct SatelliteInfo {
    double x, y, z;
};

struct Ephemeris {
    QDateTime dateTime;
    QHash<int, SatelliteInfo> satelliteInfo;
};

#endif // EPHEMERIS_H
