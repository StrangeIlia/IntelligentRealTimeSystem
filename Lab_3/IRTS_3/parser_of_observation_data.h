#ifndef PARSEROFOBSERVATIONDATA_H
#define PARSEROFOBSERVATIONDATA_H

#include <cmath>

#include <QList>
#include <QVector>
#include <QTextStream>

#include "satellite/observation_data.h"

class ParserOfObservationData
{
public:
    ParserOfObservationData();
    RecieverData *convertStream(QTextStream &stream);
};

#endif // PARSEROFOBSERVATIONDATA_H
