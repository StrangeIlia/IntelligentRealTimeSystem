#include "parser_of_observation_data.h"

ParserOfObservationData::ParserOfObservationData()
{

}

RecieverData *ParserOfObservationData::convertStream(QTextStream &stream) {
    RecieverData *result = new RecieverData;

    bool ok;
    QString str;

    /// Пропуск не интересных строк
    while(!stream.atEnd()) {
        str = stream.readLine();
        if(str.contains("APPROX POSITION XYZ")) break;
    }

    result->X = str.left(14).toInt(&ok); if(!ok) throw std::invalid_argument("File is broken"); str.remove(0, 14);
    result->Y = str.left(14).toInt(&ok); if(!ok) throw std::invalid_argument("File is broken"); str.remove(0, 14);
    result->Z = str.left(14).toInt(&ok); if(!ok) throw std::invalid_argument("File is broken"); str.remove(0, 14);

    /// Пропуск не интересных строк
    while(!stream.atEnd()) {
        str = stream.readLine();
        if(str.contains("TYPES OF OBSERV")) break;
    }

    if(stream.atEnd()) throw std::invalid_argument("File is broken");

    int observationCount = str.left(6).toInt(&ok); if(!ok) throw std::invalid_argument("File is broken"); str.remove(0, 6);
    QList<QString> observations;
    int observationNumber = 0;
    while(observationNumber != observationCount) {
        ++observationNumber;
        if(observationNumber % 9 == 0) {
            str = stream.readLine();
            str.remove(0, 6);
            if(stream.atEnd()) throw std::invalid_argument("File is broken");
        }
        str.remove(0, 4); /// Удаляем первые 4 пробела
        observations.append(str.left(2)); str.remove(0, 2);
    }

    while(!stream.atEnd()) {
        str = stream.readLine();
        if(str.contains("END OF HEADER")) break;
    }

    if(stream.atEnd()) throw std::invalid_argument("File is broken");

    struct Observe {
        double value = 0;
        uchar sync = 0;
        uchar signalStrength = 0;
    };

    struct Satellite {
        uchar type;
        uchar number;
    };

    class RowReader {
        int observationCount;
        int position = 0;
        QTextStream &stream;
        QString line;
    public:
        RowReader(int observationCount, QTextStream &_stream) : stream(_stream) {
            this->observationCount = observationCount;
            this->position = observationCount;
        }

        Observe next() {
            if(position % 5 == 0) {
                if(stream.atEnd()) throw std::invalid_argument("File is broken");
                line = stream.readLine();
            }
            if (position == observationCount) {
                if(stream.atEnd()) throw std::invalid_argument("File is broken");
                line = stream.readLine();
                position = 0;
            }

            ++position;
            Observe result;

            if(line != "") {
                bool ok;
                result.value = line.left(14).toDouble(&ok); line.remove(0, 14);
                if(!ok) result.value = 0;
                result.sync = line.left(1).toInt(&ok); line.remove(0, 1);
                if(!ok) result.sync = 0;
                result.signalStrength = line.left(1).toInt(&ok); line.remove(0, 1);
                if(!ok) result.signalStrength = 0;
            }

            return result;
        }
    };

    RowReader reader(observationCount, stream);
    int century = QDate::currentDate().year();
    century /= 100;
    century *= 100;

    while(!stream.atEnd()) {
        str = stream.readLine();

        QList<Satellite> satellits;

        // 1X I2
        int year = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        int month = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        int day = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        int hour = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        int minute = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        // F11
        double second = str.left(11).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 11);

        ObservationData *data = new ObservationData;
        data->dateTime.setDate(QDate(century + year, month, day));
        double ms = second - std::ceil(second); ms *= 1000;
        data->dateTime.setTime(QTime(hour, minute, second, ms));

        /// Удаляем флаг эпохи
        str.remove(0, 3);
        /// Число спутников в эпохе
        int satelliteCount = str.left(3).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 3);
        int satellitNumber = 0;
        while(satellitNumber != satelliteCount) {
            if(satellitNumber != 0 && satellitNumber % 12 == 0) {
                str = stream.readLine();
                if(stream.atEnd()) std::invalid_argument("File is broken");
                /// Удаляем 32 начальных пробела
                str.remove(0, 32);
            }
            ++satellitNumber;

            Satellite satellite;
            satellite.type = str.left(1).front().digitValue(); str.remove(0, 1);
            satellite.number = str.left(2).toInt(&ok); if(!ok) std::invalid_argument("File is broken"); str.remove(0, 2);
            satellits.append(satellite);
        }

        for(int i = 0; i != satelliteCount; ++i) {
            bool hasData = false;
            ObservationDataRow row;
            for(int j = 0; j != observationCount; ++j) {
                auto observe = reader.next();
                if(observations[j] == "C1") {
                    row.c1 = observe.value;
                    hasData = true;
                }
            }

            /// Можно было бы считать все спутники, но зачем хд
            if(hasData && satellits[i].type != 'G')  {
                data->gps[satellits[i].number] = row;
            }
        }

        if(data->gps.empty()) delete data;
        else result->observations.append(data);
    }

    return result;
}
