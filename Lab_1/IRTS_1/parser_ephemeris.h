#ifndef PARSEREPHEMERIS_H
#define PARSEREPHEMERIS_H

#include <cmath>
#include <QVector>
#include <QTextStream>
#include "satellite/ephemeris.h"

class ParserEphemeris
{   
public:
    ParserEphemeris();

    QVector<Ephemeris*> convertStream(QTextStream &stream) const;
};

#endif // PARSEREPHEMERIS_H
