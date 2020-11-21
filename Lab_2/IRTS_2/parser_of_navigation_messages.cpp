#include "parser_of_navigation_messages.h"

ParserOfNavigationMessages::ParserOfNavigationMessages() {

}

QString ParserOfNavigationMessages::getLine(QTextStream &stream) const {
    if(stream.atEnd()) throw std::invalid_argument("Stream is broken");
    QString str = stream.readLine();
    str.replace("D", "e");
    return str;
}

NavigationMessage* ParserOfNavigationMessages::convertBlock(QTextStream &stream) const {
    QString str;
    NavigationMessage *result = new NavigationMessage();

    bool ok;

    /// Переход на новую строку нас не очень интересует
    str.remove("\n");
    str.replace("D", "e");

    static int intLength = 2;
    static int floatLength = 5;
    static int doubleLength = 19;

    str = getLine(stream);
    result->satelliteNumber = str.left(intLength).toInt(&ok);  str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
    int year    = str.left(intLength).toInt(&ok);              str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
    int month   = str.left(intLength).toInt(&ok);              str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
    int day     = str.left(intLength).toInt(&ok);              str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
    int hour    = str.left(intLength).toInt(&ok);              str.remove(0, intLength + 1);    if(!ok) throw std::invalid_argument("File is broken");
    int minute  = str.left(intLength).toInt(&ok);              str.remove(0, intLength);        if(!ok) throw std::invalid_argument("File is broken");
    double second  = str.left(floatLength).toDouble(&ok);      str.remove(0, floatLength);      if(!ok) throw std::invalid_argument("File is broken");
    QDate date; date.setDate(year, month, day); date = date.addYears(2000);
    QTime time; time.setHMS(hour, minute, second, (second - std::floor(second)) * 1000);
    result->TOC.setDate(date);
    result->TOC.setTime(time);

    /// Сдвиг часов
    result->timeShift               = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    /// Скорость ухода часов
    result->timeShiftVelocity       = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    /// Ускорение ухода часов
    result->timeShiftAcceleration   = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");

    /// Считываем новую строку и удаляем первые 3 пробела в строке
    str = getLine(stream); str.remove(0, 3);
    /// IODE нас не интересует
    str.remove(0, doubleLength);
    result->CRS                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->dMeanAnomaly            = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->meanAnomaly             = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    str = getLine(stream); str.remove(0, 3);
    result->CUC                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->eccentricity            = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->CUS                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->semiMajorAxis           = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->semiMajorAxis           = std::pow(result->semiMajorAxis, 2);
    str = getLine(stream); str.remove(0, 3);
    result->TOE                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->CIC                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->ascendingNodeLongitude  = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->CIS                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    str = getLine(stream); str.remove(0, 3);
    result->inclinationAngle        = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->CRC                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->perigeeArgument         = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    result->OMEGADOT                = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    str = getLine(stream); str.remove(0, 3);
    result->IDOT                    = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");
    //stream.readLine();
    str = getLine(stream); str.remove(0, 3);
    str.remove(0, doubleLength); /// Точность положения спутника
    str.remove(0, doubleLength); /// Исправность спутника
    /// Групповая задержка сигнала
    result->TGD                     = str.left(doubleLength).toDouble(&ok);    str.remove(0, doubleLength); if(!ok) throw std::invalid_argument("File is broken");


    stream.readLine();

    return result;
}

NavigationMessage* ParserOfNavigationMessages::convertBlock(QString str) const {
    NavigationMessage *result = new NavigationMessage();

    /// Переход на новую строку нас не очень интересует
    str.remove("\n");
    str.replace("D", "e");

    static int intLength = 2;
    static int floatLength = 5;
    static int doubleLength = 19;

    result->satelliteNumber = str.left(intLength).toInt();  str.remove(0, intLength + 1);
    int year    = str.left(intLength).toInt();              str.remove(0, intLength + 1);
    int month   = str.left(intLength).toInt();              str.remove(0, intLength + 1);
    int day     = str.left(intLength).toInt();              str.remove(0, intLength + 1);
    int hour    = str.left(intLength).toInt();              str.remove(0, intLength + 1);
    int minute  = str.left(intLength).toInt();              str.remove(0, intLength);
    double second  = str.left(floatLength).toDouble();      str.remove(0, floatLength);
    QDate date; date.setDate(year, month, day); date = date.addYears(2000);
    QTime time; time.setHMS(hour, minute, second, (second - std::floor(second)) * 1000);
    result->TOC.setDate(date);
    result->TOC.setTime(time);

    /// Сдвиг часов
    result->timeShift               = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    /// Скорость ухода часов
    result->timeShiftVelocity       = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    /// Ускорение ухода часов
    result->timeShiftAcceleration   = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);

    /// Удаляем первые 3 пробела в строке
    str.remove(0, 3);
    str.remove(0, doubleLength);
    result->CRS                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->dMeanAnomaly            = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->meanAnomaly             = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    str.remove(0, 3);
    result->CUC                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->eccentricity            = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->CUS                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->semiMajorAxis           = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->semiMajorAxis           = std::pow(result->semiMajorAxis, 2);
    str.remove(0, 3);
    result->TOE                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->CIC                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->ascendingNodeLongitude  = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->CIS                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    str.remove(0, 3);
    result->inclinationAngle        = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->CRC                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->perigeeArgument         = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    result->OMEGADOT                = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);
    str.remove(0, 3);
    result->IDOT                    = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);

    str.remove(0, doubleLength); /// Коды в диапазоне L2
    str.remove(0, doubleLength); /// Номер GPS недели
    str.remove(0, doubleLength); /// Флаг данных L2 P
    str.remove(0, 3);
    str.remove(0, doubleLength); /// Точность положения спутника
    str.remove(0, doubleLength); /// Исправность спутника
    /// Групповая задержка сигнала
    result->TGD                     = str.left(doubleLength).toDouble();    str.remove(0, doubleLength);

    return result;
}

QVector<NavigationMessage*> ParserOfNavigationMessages::convertStream(QTextStream &stream) {
    /// Нас интересуют только данные спутников
    QString str; while(!stream.atEnd() && !str.contains("END OF HEADER")) str = stream.readLine();
    if(stream.atEnd()) throw std::invalid_argument("File is broken");

    QVector<NavigationMessage*> result;
    while(!stream.atEnd()) {
        result.push_back(convertBlock(stream));
    }
    return result;
}
