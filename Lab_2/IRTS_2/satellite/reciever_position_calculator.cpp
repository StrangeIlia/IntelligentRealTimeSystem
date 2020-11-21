#include "reciever_position_calculator.h"

RecieverPositionCalculator::RecieverPositionCalculator()
{

}

RecieverPosition *RecieverPositionCalculator::calculate(ObservationData *observ, SatelliteMessagesContainer *container) {
    SatellitePositionCalculator *calculator = new SatellitePositionCalculator(container);

    struct RequiredData {
        int satelliteNumber;
        QDateTime emissionTime;

        double pseudorange;
        double pseudorangeCorrected;

        SatelliteCalcData *emissionData;
        SatelliteCalcData *receiverData;

        ~RequiredData() {
            delete receiverData;
        }
    };

    /* Скорость света*/
    double c = 299792458;

    QVector<RequiredData*> satellitesInfo;
    for(auto iter = observ->gps.begin(); iter != observ->gps.end(); ++iter) {
        /// Разница между временем приема и передачи
        double dtime = iter.value().c1 / c;
        QDateTime emissionTime = observ->dateTime;
        emissionTime = emissionTime.addSecs(-dtime);
        emissionTime = emissionTime.addMSecs(-(dtime - std::ceil(dtime)) * 1000);
        auto emissionData = calculator->calculate(iter.key(), emissionTime);
        auto receiverData = calculator->calculate(iter.key(), observ->dateTime);
        if(emissionData != nullptr && receiverData != nullptr) {
            RequiredData* data = new RequiredData;
            data->satelliteNumber = iter.key();
            data->emissionTime = emissionTime;
            data->emissionData = emissionData;
            data->receiverData = receiverData;
            data->pseudorange = iter.value().c1;
            satellitesInfo.append(data);
        }
    }

    if(satellitesInfo.size() < 4) {
        for(auto info : satellitesInfo)
            delete info;
        delete calculator;
        return nullptr;
    }

    for(auto data : satellitesInfo) {
        auto message = container->nearbyMessage(data->satelliteNumber, data->emissionTime);

        double offset = data->emissionData->correctedTime;
        offset *= message->timeShiftAcceleration;
        offset += message->timeShiftVelocity;
        offset *= data->emissionData->correctedTime;
        offset += message->timeShift;
        double dTR = std::sqrt(message->semiMajorAxis);
        dTR *= std::sin(data->emissionData->eccentricAnomaly);
        dTR *= message->eccentricity;
        dTR *= -4.4428076331;
        offset += dTR;
        offset -= message->TGD;

        data->pseudorangeCorrected = data->pseudorange;
        data->pseudorangeCorrected -= offset * c;
    }

    RecieverPosition *result = new RecieverPosition;

    for(auto info : satellitesInfo)
        delete info;
    delete calculator;
    return result;
}

RecieverPosition *RecieverPositionCalculator::calculate(RecieverData *reciever, SatelliteMessagesContainer *container) {
    class Enumerator {
        int i = 0;
        RecieverData *reciever;
    public:
        Enumerator(RecieverData *reciever) {
            this->reciever = reciever;
        }

        void reset() {
            i = 0;
        }

        int next() {
            for(; i != reciever->observations.size(); ++i) {
                if(reciever->observations[i]->gps.size() > 4) {
                    return i;
                }
            }
            return -1;
        }
    };

    int observationNumber;
    Enumerator enumerator(reciever);
    while((observationNumber = enumerator.next()) != -1) {
        auto result = calculate(reciever->observations[observationNumber], container);
        if(result != nullptr) return result;
    }

    return nullptr;
}
