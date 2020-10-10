#include "main_window.h"

#include "parser_of_navigation_messages.h"
#include "parser_of_navigation_messages.h"
#include "satellite/position_calculator.h"

#include <QFile>
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    ParserOfNavigationMessages parser;
    CalculatedData *calculated = new CalculatedData;
    NavigationMessage *message = parser.convertBlock(" 8 16  2 24  0  0  0.0-2.503162249923D-05-1.477928890381D-12 0.000000000000D+00"
                                                     "    2.200000000000D+01 4.578125000000D+01 4.880917595524D-09 1.920895700696D+00"
                                                     "    2.320855855942D-06 1.822456018999D-03 2.659857273102D-06 5.153647605896D+03"
                                                     "    2.592000000000D+05-2.793967723846D-08-1.590116734767D+00-4.097819328308D-08"
                                                     "    9.618874487817D-01 3.233125000000D+02-1.346068583171D+00-8.470352824194D-09"
                                                     "    1.000041655749D-10 1.000000000000D+00 1.885000000000D+03 0.000000000000D+00"
                                                     "    2.000000000000D+00 0.000000000000D+00 4.656612873077D-09 2.200000000000D+01"
                                                     "    2.554080000000D+05 4.000000000000D+00");

    timeSinceTheBeginningOfTheWeek(calculated, message, message->TOC.time());
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

    qDebug() << calculated->satellitePosX;
    qDebug() << calculated->satellitePosY;
    qDebug() << calculated->satellitePosZ;

    delete calculated;
    delete message;

    return a.exec();
}
