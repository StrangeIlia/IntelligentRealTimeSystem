#ifndef POSITIONCALCULATOR_H
#define POSITIONCALCULATOR_H

#include "calculation.h"

#include <QMap>
#include <QHash>

class PositionCalculator
{
    typedef QMap<QTime, NavigationMessage*>         MessagesInOneDay;
    typedef QHash<QDate, MessagesInOneDay>          SatelliteMessages;
    typedef QHash<int, SatelliteMessages>           AllMessages;
    /// Ключ - номер спутника
    /// Значение - все сообщения для данного спутника
    AllMessages _messages;
public:
    PositionCalculator();

    void addMessage(NavigationMessage *message);
    void removeMessage(NavigationMessage *message);

    AllMessages messages() const;

    /// Возвращает nullptr, если не смог вычислить
    CalculatedData *calculate(int satelliteNumber, QDateTime TPC) const;
private:
    /// Это методы для дополнения
    SatelliteMessages &getSatelliteMessages(int satelliteNumber);
    MessagesInOneDay &getMessagesInOneDay(SatelliteMessages &messages, QDate date);
};

#endif // POSITIONCALCULATOR_H
