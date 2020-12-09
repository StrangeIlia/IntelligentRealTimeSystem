#ifndef OBSERVATIONDATA_H
#define OBSERVATIONDATA_H

#include <QHash>
#include <QString>
#include <QVector>
#include <QDateTime>

struct ObservationDataRow {
    double c1; /// Псевдодальность
};

struct ObservationData {
    QDateTime dateTime;
    QHash<int, ObservationDataRow> gps;
};

struct RecieverData {
    QString recieverName = "";
    double X, Y, Z; /// Примерные координаты станции
    double H, E, N; /// Сдвиги для положения антены
    QVector<ObservationData*> observations;

    ~RecieverData() {
        for(auto data : observations)
            delete data;
    }
};

#endif // OBSERVATIONDATA_H
