#include "parser_ephemeris.h"

ParserEphemeris::ParserEphemeris() {

}

QVector<Ephemeris*> ParserEphemeris::convertStream(QTextStream &stream) const {
    QString str = "123"; while(!stream.atEnd() && str.front() != "*") str = stream.readLine();

    QVector<Ephemeris*> result;
    while(!stream.atEnd()) {
        Ephemeris *ephemeris = new Ephemeris;
        /// Удаляем пустое пространство перед годом
        str.remove(0, 3);

        int yearLength = 4;
        int intLength = 2;
        int floatLength = 11;
        int doubleLength = 13;

        int year        = str.left(yearLength).toInt();     str.remove(0, yearLength + 1);
        int month       = str.left(intLength).toInt();      str.remove(0, intLength + 1);
        int day         = str.left(intLength).toInt();      str.remove(0, intLength + 1);
        int hour        = str.left(intLength).toInt();      str.remove(0, intLength + 1);
        int minute      = str.left(intLength).toInt();      str.remove(0, intLength + 1);
        double second   = str.left(floatLength).toDouble(); str.remove(0, floatLength);
        QDate date; date.setDate(year, month, day);
        QTime time; time.setHMS(hour, minute, second, (second - std::floor(second)) * 1000);
        ephemeris->dateTime.setDate(date);
        ephemeris->dateTime.setTime(time);

        str = stream.readLine();
        while(!stream.atEnd() && str.front() != "*") {
            str.remove(0, 2);
            int satelliteNumber                             = str.left(intLength).toInt();          str.remove(0, intLength + 1);
            ephemeris->satelliteInfo[satelliteNumber]       = SatelliteInfo();
            ephemeris->satelliteInfo[satelliteNumber].x     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength + 1);
            ephemeris->satelliteInfo[satelliteNumber].y     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength + 1);
            ephemeris->satelliteInfo[satelliteNumber].z     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength + 1);
            str = stream.readLine();
        }

        result.push_back(ephemeris);
    }

    return result;
}

