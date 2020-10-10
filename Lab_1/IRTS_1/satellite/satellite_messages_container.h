#ifndef SATELLITEMESSAGESCONTAINER_H
#define SATELLITEMESSAGESCONTAINER_H

#include <QMap>
#include <QHash>
#include <QTime>
#include <QDate>
#include <QDateTime>

#include "navigation_message.h"

/// Контролирует добавляемые в него записи
class SatelliteMessagesContainer
{
public:
    typedef QHash<int, NavigationMessage*>          MessagesInTime;
    typedef QMap<QTime, MessagesInTime>             MessagesInOneDay;
    typedef QHash<QDate, MessagesInOneDay>          AllMessages;

public:
    SatelliteMessagesContainer(bool memoryControl = true);
    ~SatelliteMessagesContainer();

    bool memoryControlEnabled() const;
    void setMemoryControl(bool value);

    void addMessage(NavigationMessage *message);
    void removeMessage(NavigationMessage *message);
    AllMessages messages() const;

    /// Возвращает сообщение
    /// Если такого сообщения нет, вернет nullptr
    NavigationMessage *message(int satelliteNumber, QDateTime tpc) const;
    /// Возвращает ближайшее сообщение, полученное в тот же день
    /// Если в этот день не было сообщений, вернет nullptr
    NavigationMessage *nearbyMessage(int satelliteNumber, QDateTime tpc) const;

private:
    /// Ключ - номер спутника
    /// Значение - все сообщения для данного спутника
    AllMessages _messages;
    bool _memoryControl = true;
    /// Это методы для дополнения
    AllMessages &getSatelliteMessages(int satelliteNumber);
    MessagesInOneDay &getMessagesInOneDay(AllMessages &messages, QDate date);
};

#endif // SATELLITEMESSAGESCONTAINER_H
