#include "parser_ephemeris.h"

ParserEphemeris::ParserEphemeris() {

}

QVector<Ephemeris*> ParserEphemeris::convertStream(QTextStream &stream) const {
    QString str = "123"; while(!stream.atEnd() && str.front() != "*") str = stream.readLine();
    if(stream.atEnd()) throw std::invalid_argument("File is broken");

    QVector<Ephemeris*> result;
    while(!stream.atEnd()) {
        Ephemeris *ephemeris = new Ephemeris;
        /// Удаляем пустое пространство перед годом
        str.remove(0, 3);

        bool ok;

        int yearLength = 4;
        int intLength = 2;
        int floatLength = 11;
        int doubleLength = 13;

        int year        = str.left(yearLength).toInt(&ok);     str.remove(0, yearLength + 1);   if(!ok) throw std::invalid_argument("File is broken");
        int month       = str.left(intLength).toInt(&ok);      str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
        int day         = str.left(intLength).toInt(&ok);      str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
        int hour        = str.left(intLength).toInt(&ok);      str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
        int minute      = str.left(intLength).toInt(&ok);      str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
        double second   = str.left(floatLength).toDouble(&ok); str.remove(0, floatLength);      if(!ok) throw std::invalid_argument("File is broken");
        QDate date; date.setDate(year, month, day);
        QTime time; time.setHMS(hour, minute, second, (second - std::floor(second)) * 1000);
        ephemeris->dateTime.setDate(date);
        ephemeris->dateTime.setTime(time);

        str = stream.readLine();
        while(!stream.atEnd() && str.front() != "*") {
            str.remove(0, 2);
            int satelliteNumber                             = str.left(intLength).toInt(&ok);          str.remove(0, intLength + 1);       if(!ok) throw std::invalid_argument("File is broken");
            ephemeris->satelliteInfo[satelliteNumber]       = SatelliteInfo();
            ephemeris->satelliteInfo[satelliteNumber].x     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
            ephemeris->satelliteInfo[satelliteNumber].y     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
            ephemeris->satelliteInfo[satelliteNumber].z     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
            str = stream.readLine();
        }

        result.push_back(ephemeris);
    }

    return result;
}

