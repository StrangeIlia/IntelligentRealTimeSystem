#include "calculation.h"

void timeSinceTheBeginningOfTheWeek(SatelliteCalcData *calculated, NavigationMessage *message, QTime TPC) {
    double dayNumber = std::floor(message->TOE / 86400);
    calculated->time  = dayNumber * 86400;
    calculated->time += TPC.hour() * 3600;
    calculated->time += TPC.minute() * 60;
    calculated->time += TPC.second();
    /// Чтоб было чуть - чуть поточней хд
    calculated->time += TPC.msec() / 1000.0;
}

void timeSinceEra(SatelliteCalcData *calculated, NavigationMessage *message) {
    calculated->correctedTime = calculated->time - message->TOE;
    if(calculated->correctedTime > 302400)         calculated->correctedTime -= 604800;
    else if(calculated->correctedTime < -302400)   calculated->correctedTime += 604800;
}

void correctedAverageMovement(SatelliteCalcData *calculated, NavigationMessage *message) {
    static double m = 3.986005e14;
    double a_3 = std::pow(message->semiMajorAxis, 3);
    calculated->correctedAverageMovement = std::sqrt(m / a_3) + message->dMeanAnomaly;
}

void meanAnomaly(SatelliteCalcData *calculated, NavigationMessage *message) {
    calculated->meanAnomaly = message->meanAnomaly + calculated->correctedAverageMovement * calculated->correctedTime;
}

void eccentricAnomaly(SatelliteCalcData *calculated, NavigationMessage *message) {
    double eps = 1e-8; //std::numeric_limits<double>::epsilon();
    double dEA = std::numeric_limits<double>::max();
    calculated->eccentricAnomaly = calculated->meanAnomaly;
    while(std::abs(dEA) > eps) {
        dEA  = calculated->meanAnomaly - calculated->eccentricAnomaly;
        dEA += message->eccentricity * std::sin(calculated->eccentricAnomaly);
        dEA /= 1 - message->eccentricity * std::cos(calculated->eccentricAnomaly);
        calculated->eccentricAnomaly += dEA;
    }
}

void trueAnomaly(SatelliteCalcData *calculated, NavigationMessage *message) {
    double tSin = std::sqrt(1 - std::pow(message->eccentricity, 2));
    tSin *= std::sin(calculated->eccentricAnomaly);
    double tCos = std::cos(calculated->eccentricAnomaly) - message->eccentricity;
    calculated->trueAnomaly = std::atan2(tSin, tCos);
}

void latitudeArgument(SatelliteCalcData *calculated, NavigationMessage *message) {
    calculated->latitudeArgument = calculated->trueAnomaly + message->perigeeArgument;
}

void correctedLatitudeArgument(SatelliteCalcData *calculated, NavigationMessage *message) {
    double dUk = message->CUC * std::cos(2 * calculated->latitudeArgument);
    dUk += message->CUS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedLatitudeArgument = calculated->latitudeArgument + dUk;
}

void correctedRadiusVector(SatelliteCalcData *calculated, NavigationMessage *message) {
    double dRk = message->CRC * std::cos(2 * calculated->latitudeArgument);
    dRk += message->CRS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedRadiusVector = message->semiMajorAxis * (1 - message->eccentricity * std::cos(calculated->eccentricAnomaly));
    calculated->correctedRadiusVector += dRk;
}

void correctedInclinationAngle(SatelliteCalcData *calculated, NavigationMessage *message) {
    double dIk = message->CIC * std::cos(2 * calculated->latitudeArgument);
    dIk += message->CIS * std::sin(2 * calculated->latitudeArgument);
    calculated->correctedInclinationAngle  = message->inclinationAngle;
    calculated->correctedInclinationAngle += dIk;
    calculated->correctedInclinationAngle += message->IDOT * calculated->correctedTime;
}

void orbitalSatellitePosition(SatelliteCalcData *calculated) {
    calculated->orbitalSatelliteX = calculated->correctedRadiusVector * std::cos(calculated->correctedLatitudeArgument);
    calculated->orbitalSatelliteY = calculated->correctedRadiusVector * std::sin(calculated->correctedLatitudeArgument);
}

void correcntedLongitude(SatelliteCalcData *calculated, NavigationMessage *message) {
    static double w3 = 7.2921151467e-5;
    calculated->correctedLongitude  = message->ascendingNodeLongitude;
    calculated->correctedLongitude += (message->OMEGADOT - w3) * calculated->correctedTime;
    calculated->correctedLongitude -= w3 * message->TOE;
}

void satellitePosition(SatelliteCalcData *calculated) {
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


void offset(ReceiverCalcData *result, SatelliteCalcData *satellite, NavigationMessage *message) {
    result->offset  = message->timeShiftAcceleration * satellite->correctedTime;
    result->offset += message->timeShiftVelocity;
    result->offset *= satellite->correctedTime;
    result->offset += message->timeShift;
    double dTR = std::sqrt(message->semiMajorAxis);
    dTR *= std::sin(satellite->eccentricAnomaly);
    dTR *= message->eccentricity;
    dTR *= -4.4428076331;
    result->offset += dTR;
    result->offset -= message->TGD;
}
