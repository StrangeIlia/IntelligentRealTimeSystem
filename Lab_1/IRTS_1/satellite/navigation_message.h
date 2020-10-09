#ifndef NAVIGATION_MESSAGE_H
#define NAVIGATION_MESSAGE_H

#include <QDateTime>

struct NavigationMessage {
    int satelliteNumber = -1;       // Номер спутника (Не используется в рассчетах)
    double dMeanAnomaly;            // dn
    double meanAnomaly;             // M0
    double eccentricity;            // e0
    /// В файлах у нас sqrt(A), но записываем в структуру A
    double semiMajorAxis;           // A

    double CIC;
    double CRC;
    double CIS;
    double CRS;
    double CUC;
    double CUS;

    double ascendingNodeLongitude;  // Omega0 (Омега большая)
    double perigeeArgument;         // w (омега маленькая)
    double inclinationAngle;        // I0

    double OMEGADOT;
    double IDOT;
    double TOE;
    int gpsWeekNumber;              // Номер GPS недели (Не используется в рассчетах)
    QDateTime TOC;                  // Время привязки данных (Не используется в рассчетах)

    double timeShift;
    double timeShiftVelocity;
    double timeShiftAcceleration;
    double correctedTOE; /// Без корректирования равен TOE
};

#endif // NAVIGATION_MESSAGE_H
