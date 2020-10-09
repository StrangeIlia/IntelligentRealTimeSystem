#include "calculation.h"

void correctTOE(NavigationMessage *ephemeric) {
    ephemeric->correctedTOE    += ephemeric->timeShift;
    ephemeric->correctedTOE    += ephemeric->timeShiftVelocity * ephemeric->TOE;
    ephemeric->correctedTOE    += ephemeric->timeShiftAcceleration * std::pow(ephemeric->TOE, 2) / 2;
}

void timeSinceTheBeginningOfTheWeek(CalculatedData *calculated, NavigationMessage *ephemeric, QTime TPC) {
    double dayNumber = std::floor(ephemeric->correctedTOE / 86400);
    calculated->time  = dayNumber * 86400;
    calculated->time += TPC.hour() * 3600;
    calculated->time += TPC.minute() * 60;
    calculated->time += TPC.second();
    /// Чтоб было чуть - чуть поточней хд
    calculated->time += TPC.msec() / 1000.0;
}

void timeSinceEra(CalculatedData *calculated, NavigationMessage *ephemeric) {
    calculated->correctedTime = calculated->time - ephemeric->correctedTOE;
    if(calculated->correctedTime > 302400)         calculated->correctedTime -= 604800;
    else if(calculated->correctedTime < -302400)   calculated->correctedTime += 604800;
}

void correctedAverageMovement(CalculatedData *calculated, NavigationMessage *ephemeric) {
    static double m = 3.986005e14;
    double a_3 = std::pow(ephemeric->semiMajorAxis, 3);
    calculated->correctedAverageMovement = std::sqrt(m / a_3) + ephemeric->dMeanAnomaly;
}

void meanAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric) {
    calculated->meanAnomaly = ephemeric->meanAnomaly + calculated->correctedAverageMovement * calculated->correctedTime;
}

void eccentricAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric) {
    double eps = 1e-8; //std::numeric_limits<double>::epsilon();
    double dEA = std::numeric_limits<double>::max();
    calculated->eccentricAnomaly = calculated->meanAnomaly;
    while(std::abs(dEA) > eps) {
        dEA  = calculated->meanAnomaly - calculated->eccentricAnomaly;
        dEA += ephemeric->eccentricity * std::sin(calculated->eccentricAnomaly);
        dEA /= 1 - ephemeric->eccentricity * std::cos(calculated->eccentricAnomaly);
        calculated->eccentricAnomaly += dEA;
    }
}

void trueAnomaly(CalculatedData *calculated, NavigationMessage *ephemeric) {
    double tSin = std::sqrt(1 - std::pow(ephemeric->eccentricity, 2));
    tSin *= std::sin(calculated->eccentricAnomaly);
    double tCos = std::cos(calculated->eccentricAnomaly) - ephemeric->eccentricity;
    calculated->trueAnomaly = std::atan2(tSin, tCos);
}

void latitudeArgument(CalculatedData *calculated, NavigationMessage *ephemeric) {
    calculated->latitudeArgument = calculated->trueAnomaly + ephemeric->perigeeArgument;
}

void correctedLatitudeArgument(CalculatedData *calculated, NavigationMessage *ephemeric) {
    double dUk = ephemeric->CUC * std::cos(2 * calculated->latitudeArgument);
    dUk += ephemeric->CUS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedLatitudeArgument = calculated->latitudeArgument + dUk;
}

void correctedRadiusVector(CalculatedData *calculated, NavigationMessage *ephemeric) {
    double dRk = ephemeric->CRC * std::cos(2 * calculated->latitudeArgument);
    dRk += ephemeric->CRS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedRadiusVector = ephemeric->semiMajorAxis * (1 - ephemeric->eccentricity * std::cos(calculated->eccentricAnomaly));
    calculated->correctedRadiusVector += dRk;
}

void correctedInclinationAngle(CalculatedData *calculated, NavigationMessage *ephemeric) {
    double dIk = ephemeric->CIC * std::cos(2 * calculated->latitudeArgument);
    dIk += ephemeric->CIS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedInclinationAngle  = ephemeric->inclinationAngle;
    calculated->correctedInclinationAngle += dIk;
    calculated->correctedInclinationAngle += ephemeric->IDOT * calculated->correctedTime;
}

void orbitalSatellitePosition(CalculatedData *calculated) {
    calculated->orbitalSatelliteX = calculated->correctedRadiusVector * std::cos(calculated->correctedLatitudeArgument);
    calculated->orbitalSatelliteY = calculated->correctedRadiusVector * std::sin(calculated->correctedLatitudeArgument);
}

void correcntedLongitude(CalculatedData *calculated, NavigationMessage *ephemeric) {
    static double w3 = 7.2921151467e-5;
    calculated->correctedLongitude  = ephemeric->ascendingNodeLongitude;
    calculated->correctedLongitude += (ephemeric->OMEGADOT - w3) * calculated->correctedTime;
    calculated->correctedLongitude -= w3 * ephemeric->correctedTOE;
}

void satellitePosition(CalculatedData *calculated) {
    double cosLong = std::cos(calculated->correctedLongitude);
    double sinLong = std::sin(calculated->correctedLongitude);
    double cosRad = std::cos(calculated->correctedInclinationAngle);
    double sinRad = std::sin(calculated->correctedInclinationAngle);
    calculated->satellitePosX  = calculated->orbitalSatelliteX * cosLong;
    calculated->satellitePosX -= calculated->orbitalSatelliteY * cosRad * sinLong;
    calculated->satellitePosY  = calculated->orbitalSatelliteX * sinLong;
    calculated->satellitePosY += calculated->orbitalSatelliteY * cosRad * cosLong;
    calculated->satellitePosZ  = calculated->orbitalSatelliteY * sinRad;
}
