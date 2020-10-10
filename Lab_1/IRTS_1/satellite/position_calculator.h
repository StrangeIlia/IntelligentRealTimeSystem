#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include "calculation.h"
#include "satellite_messages_container.h"

class PositionCalculator
{
    SatelliteMessagesContainer *_container;
public:
    PositionCalculator(SatelliteMessagesContainer *container = nullptr);

    SatelliteMessagesContainer *container() const;
    void setContainer(SatelliteMessagesContainer *container);

    /// Возвращает nullptr, если не смог вычислить
    CalculatedData *calculate(int satelliteNumber, QDateTime TPC) const;
};

#endif // POSITIONCALCULATOR_H
