#ifndef RECIEVERPOSITIONCALCULATOR_H
#define RECIEVERPOSITIONCALCULATOR_H

#include "observation_data.h"
#include "satellite_position_calculator.h"

struct RecieverPosition {
    double x = 0, y = 0, z = 0, dt = 0;
};

class RecieverPositionCalculator
{
public:
    RecieverPositionCalculator();

    RecieverPosition *calculate(ObservationData *data, SatelliteMessagesContainer *container);
    RecieverPosition *calculate(RecieverData *reciever, SatelliteMessagesContainer *container);
};

#endif // RECIEVERPOSITIONCALCULATOR_H
