#include "calculate_task.h"

CalculateTask::CalculateTask(SatelliteMessagesContainer *container) {
    setAutoDelete(false);
    _containerMessages = container;
    _contanerResults = new EphemerisContainer();
}

void CalculateTask::run() {
    PositionCalculator calculator(_containerMessages);
    for(auto messagesInOneDay : _containerMessages->messages()) {
        for(auto messagesInOneTime : messagesInOneDay) {
            for(auto message : messagesInOneTime) {
                CalculatedData *data = calculator.calculate(message->satelliteNumber, message->TOC);
                Ephemeris *ephemeris = new Ephemeris();
                ephemeris->dateTime = message->TOC;
                SatelliteInfo info;
                info.x = data->satellitePosX;
                info.y = data->satellitePosY;
                info.z = data->satellitePosZ;
                ephemeris->satelliteInfo[message->satelliteNumber] = info;
                if(!_contanerResults->addEphemeris(ephemeris)) {
                    delete ephemeris;
                }
                delete data;
            }
        }
    }
    emit finished();
}

bool CalculateTask::ok() const {
    return amountOfProcessedData() == _containerMessages->messageCount();
}

int CalculateTask::amountOfProcessedData() const {
    return _contanerResults->ephemerisCount();
}

EphemerisContainer *CalculateTask::results() const {
    return _contanerResults;
}

SatelliteMessagesContainer *CalculateTask::messageContainer() const {
    return _containerMessages;
}
