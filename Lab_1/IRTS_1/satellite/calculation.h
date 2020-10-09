#ifndef SATELLITE_H
#define SATELLITE_H

#include <cmath>
#include <QTime>

#include "navigation_message.h"

struct CalculatedData {
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

void correctTOE(NavigationMessage *ephemeric);
void timeSinceTheBeginningOfTheWeek(CalculatedData *calculated, NavigationMessage *ephemeric, QTime TPC);
void timeSinceEra(CalculatedData *calculated, NavigationMessage *ephemeric);
void correctedAverageMovement(CalculatedData *calculated, NavigationMessage *ephemeric);
void meanAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric);
void eccentricAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric);
void trueAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric);
void latitudeArgument(CalculatedData *calculated, NavigationMessage *ephemeric);
void correctedLatitudeArgument(CalculatedData *calculated, NavigationMessage *ephemeric);
void correctedRadiusVector(CalculatedData *calculated, NavigationMessage *ephemeric);
void correctedInclinationAngle(CalculatedData *calculated, NavigationMessage *ephemeric);
void orbitalSatellitePosition(CalculatedData *calculated);
void correcntedLongitude(CalculatedData *calculated, NavigationMessage *ephemeric);
void satellitePosition(CalculatedData *calculated);

#endif // SATELLITE_H
