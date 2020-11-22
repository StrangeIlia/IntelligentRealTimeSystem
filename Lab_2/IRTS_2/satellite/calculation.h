#ifndef SATELLITE_H
#define SATELLITE_H

#include <cmath>
#include <QTime>

#include "navigation_message.h"

struct SatelliteCalcData {
    double time;
    double correctedTime;
    double correctedAverageMovement; // n
    double meanAnomaly; // Mk
    double eccentricAnomaly; // En
    double trueAnomaly; // teta k
    double latitudeArgument; //Фк
    double correctedLatitudeArgument; // Uk
    double correctedRadiusVector; //rk
    double correctedInclinationAngle; // Ik
    double orbitalSatelliteX; // XФк
    double orbitalSatelliteY; // YФк
    double correctedLongitude; // Omeagek
    double satellitePosX; // Xsvk
    double satellitePosY; // Ysvk
    double satellitePosZ; // Ysvk
};

void timeSinceTheBeginningOfTheWeek(SatelliteCalcData *calculated, NavigationMessage *message, QTime TPC);
void timeSinceEra(SatelliteCalcData *calculated, NavigationMessage *message);
void correctedAverageMovement(SatelliteCalcData *calculated, NavigationMessage *message);
void meanAnomaly(SatelliteCalcData *calculated, NavigationMessage *message);
void eccentricAnomaly(SatelliteCalcData *calculated, NavigationMessage *message);
void trueAnomaly(SatelliteCalcData *calculated, NavigationMessage *message);
void latitudeArgument(SatelliteCalcData *calculated, NavigationMessage *message);
void correctedLatitudeArgument(SatelliteCalcData *calculated, NavigationMessage *message);
void correctedRadiusVector(SatelliteCalcData *calculated, NavigationMessage *message);
void correctedInclinationAngle(SatelliteCalcData *calculated, NavigationMessage *message);
void orbitalSatellitePosition(SatelliteCalcData *calculated);
void correcntedLongitude(SatelliteCalcData *calculated, NavigationMessage *message);
void satellitePosition(SatelliteCalcData *calculated);

#endif // SATELLITE_H
