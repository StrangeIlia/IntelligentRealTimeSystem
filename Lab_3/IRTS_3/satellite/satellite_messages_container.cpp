#include "satellite_messages_container.h"

SatelliteMessagesContainer::SatelliteMessagesContainer(bool memoryControl) : _memoryControl(memoryControl) {

}

SatelliteMessagesContainer::~SatelliteMessagesContainer() {
    if(_memoryControl) {
        for(auto messagesInDay : _messages) {
            for(auto messagesInTime : messagesInDay) {
                for(auto message : messagesInTime)
                    delete message;
            }
        }
    }
}

bool SatelliteMessagesContainer::memoryControlEnabled() const {
    return _memoryControl;
}

void SatelliteMessagesContainer::setMemoryControl(bool value) {
    _memoryControl = value;
}

void SatelliteMessagesContainer::addMessage(NavigationMessage *message) {
    if(!_messages.contains(message->TOC.date())) {
        _messages.insert(message->TOC.date(), MessagesInOneDay());
    }
    auto &messagesInOneDay = _messages[message->TOC.date()];
    if(!messagesInOneDay.contains(message->TOC.time())) {
        messagesInOneDay.insert(message->TOC.time(), MessagesInTime());
    }
    auto &messagesInTime = messagesInOneDay[message->TOC.time()];
    messagesInTime[message->satelliteNumber] = message;
}

void SatelliteMessagesContainer::removeMessage(NavigationMessage *message) {
    if(!_messages.contains(message->TOC.date())) return;
    auto &messagesInOneDay = _messages[message->TOC.date()];
    if(!messagesInOneDay.contains(message->TOC.time())) return;
    auto &messagesInTime = messagesInOneDay[message->TOC.time()];
    messagesInTime.remove(message->satelliteNumber);
    if(messagesInTime.empty()) messagesInOneDay.remove(message->TOC.time());
    if(messagesInOneDay.empty()) _messages.remove(message->TOC.date());
}

SatelliteMessagesContainer::AllMessages SatelliteMessagesContainer::messages() const {
    return _messages;
}

int SatelliteMessagesContainer::messageCount() const {
    int count = 0;
    for(auto messagesInDay : _messages) {
        for(auto messagesInTime : messagesInDay) {
            count += messagesInTime.size();
        }
    }
    return count;
}

NavigationMessage *SatelliteMessagesContainer::message(int satelliteNumber, QDateTime tpc) const {
    if(!_messages.contains(tpc.date())) return nullptr;
    auto &messagesInOneDay = _messages[tpc.date()];
    if(!messagesInOneDay.contains(tpc.time())) return nullptr;
    auto &messagesInTime = messagesInOneDay[tpc.time()];
    if(!messagesInTime.contains(satelliteNumber)) return nullptr;
    return messagesInTime[satelliteNumber];
}

NavigationMessage *SatelliteMessagesContainer::nearbyMessage(int satelliteNumber, QDateTime tpc) const {
    if(!_messages.contains(tpc.date())) return nullptr;
    auto &messagesInOneDay = _messages[tpc.date()];
    auto greaterTime = messagesInOneDay.lowerBound(tpc.time());
    auto lowerTime = greaterTime == messagesInOneDay.begin() ? greaterTime : greaterTime - 1;
    if(greaterTime != messagesInOneDay.end()) ++greaterTime;
    while(lowerTime != greaterTime) {
        if(lowerTime.value().contains(satelliteNumber)) {
            break;
        }
        ++lowerTime;
    }
    if(lowerTime == greaterTime) return nullptr;
    auto bestVariant = lowerTime;
    int diff = std::abs(tpc.time().secsTo(bestVariant.key()));
    while(++lowerTime != greaterTime) {
        if(lowerTime.value().contains(satelliteNumber)) {
            int value = std::abs(tpc.time().secsTo(lowerTime.key()));
            if(value < diff) {
                diff = value;
                bestVariant = lowerTime;
            }
        }
    }
    return bestVariant.value()[satelliteNumber];
}
