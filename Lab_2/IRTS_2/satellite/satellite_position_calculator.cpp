#include "satellite_position_calculator.h"

SatellitPositionCalculator::SatellitPositionCalculator(SatelliteMessagesContainer *container) {
    _container = container;
}

SatelliteMessagesContainer *SatellitPositionCalculator::container() const {
    return _container;
}

void SatellitPositionCalculator::setContainer(SatelliteMessagesContainer *container) {
    _container = container;
}

SatelliteCalcData *SatellitPositionCalculator::calculate(int satelliteNumber, QDateTime TPC) const {
    if(_container == nullptr) return nullptr;
    NavigationMessage *message = _container->nearbyMessage(satelliteNumber, TPC);
    if(message == nullptr) return nullptr;

    SatelliteCalcData *calculated = new SatelliteCalcData;
    timeSinceTheBeginningOfTheWeek(calculated, message, TPC.time());
    timeSinceEra(calculated, message);
    correctedAverageMovement(calculated, message);
    meanAnomaly(calculated, message);
    eccentricAnomaly(calculated, message);
    trueAnomaly(calculated, message);
    latitudeArgument(calculated, message);
    correctedLatitudeArgument(calculated, message);
    correctedRadiusVector(calculated, message);
    correctedInclinationAngle(calculated, message);
    orbitalSatellitePosition(calculated);
    correcntedLongitude(calculated, message);
    satellitePosition(calculated);

    return calculated;
}
