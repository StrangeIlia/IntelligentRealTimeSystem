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

void timeSinceTheBeginningOfTheWeek(CalculatedData *calculated, NavigationMessage *message, QTime TPC);
void timeSinceEra(CalculatedData *calculated, NavigationMessage *message);
void correctedAverageMovement(CalculatedData *calculated, NavigationMessage *message);
void meanAnomaly(CalculatedData *calculated, NavigationMessage *message);
void eccentricAnomaly(CalculatedData *calculated, NavigationMessage *message);
void trueAnomaly(CalculatedData *calculated, NavigationMessage *message);
void latitudeArgument(CalculatedData *calculated, NavigationMessage *message);
void correctedLatitudeArgument(CalculatedData *calculated, NavigationMessage *message);
void correctedRadiusVector(CalculatedData *calculated, NavigationMessage *message);
void correctedInclinationAngle(CalculatedData *calculated, NavigationMessage *message);
void orbitalSatellitePosition(CalculatedData *calculated);
void correcntedLongitude(CalculatedData *calculated, NavigationMessage *message);
void satellitePosition(CalculatedData *calculated);

#endif // SATELLITE_H
