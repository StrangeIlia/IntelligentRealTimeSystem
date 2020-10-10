#include "position_calculator.h"

PositionCalculator::PositionCalculator() {

}

void PositionCalculator::addMessage(NavigationMessage *message) {
    auto satelliteMessages = getSatelliteMessages(message->satelliteNumber);
    auto messagesInOneDay = getMessagesInOneDay(satelliteMessages, message->TOC.date());
    messagesInOneDay[message->TOC.time()] = message;
}

void PositionCalculator::removeMessage(NavigationMessage *message) {
    if(!_messages.contains(message->satelliteNumber)) return;
    auto &satelliteMessages = _messages[message->satelliteNumber];
    if(!satelliteMessages.contains(message->TOC.date())) return;
    auto &messagesInOneDay = satelliteMessages[message->TOC.date()];
    messagesInOneDay.remove(message->TOC.time());
    if(messagesInOneDay.empty()) satelliteMessages.remove(message->TOC.date());
    if(satelliteMessages.empty()) _messages.remove(message->satelliteNumber);
}

PositionCalculator::AllMessages PositionCalculator::messages() const {
    return _messages;
}

CalculatedData *PositionCalculator::calculate(int satelliteNumber, QDateTime TPC) const {
    if(!_messages.contains(satelliteNumber)) return nullptr;
    auto &satelliteMessages = _messages[satelliteNumber];
    if(!satelliteMessages.contains(TPC.date())) return nullptr;
    auto &messagesInOneDay = satelliteMessages[TPC.date()];
    auto greaterTime = messagesInOneDay.lowerBound(TPC.time());
    auto lowerTime = greaterTime == messagesInOneDay.begin() ? greaterTime : greaterTime - 1;
    auto bestVariant = lowerTime;
    int diff = std::abs(TPC.time().secsTo(bestVariant.key()));
    ++greaterTime;
    while(++lowerTime != greaterTime) {
        int value = std::abs(TPC.time().secsTo(lowerTime.key()));
        if(value < diff) {
            diff = value;
            bestVariant = lowerTime;
        }
    }

    CalculatedData *calculated = new CalculatedData;
    NavigationMessage *message = bestVariant.value();

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

PositionCalculator::SatelliteMessages &PositionCalculator::getSatelliteMessages(int satelliteNumber) {
    if(!_messages.contains(satelliteNumber)) {
        _messages[satelliteNumber] = SatelliteMessages();
    }
    return _messages[satelliteNumber];
}

PositionCalculator::MessagesInOneDay &PositionCalculator::getMessagesInOneDay(SatelliteMessages &messages, QDate date) {
    if(!messages.contains(date)) {
        messages[date] = MessagesInOneDay();
    }
    return messages[date];
}
