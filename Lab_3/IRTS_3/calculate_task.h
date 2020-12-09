#ifndef CALCULATETASK_H
#define CALCULATETASK_H

#include <QObject>
#include <QRunnable>

#include "satellite/ephemeris_container.h"
#include "satellite/satellite_position_calculator.h"

class CalculateTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    CalculateTask(SatelliteMessagesContainer *messageContainer);

    void run() override;

    /// Возвращает истина, если чтение успешно
    bool ok() const;

    int amountOfProcessedData() const;
    EphemerisContainer *results() const;
    SatelliteMessagesContainer *messageContainer() const;

signals:
    void finished();

private:
    EphemerisContainer *_contanerResults;
    SatelliteMessagesContainer *_containerMessages;
};


#endif // CALCULATETASK_H
