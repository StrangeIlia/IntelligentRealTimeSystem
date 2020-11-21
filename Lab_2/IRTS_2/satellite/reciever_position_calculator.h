#ifndef RECIEVERPOSITIONCALCULATOR_H
#define RECIEVERPOSITIONCALCULATOR_H

#include "calculation.h"
#include "observation_data.h"
#include "satellite_messages_container.h"

#include "math/matrix/matrix_operations.hpp"

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
