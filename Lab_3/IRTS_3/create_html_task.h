#ifndef CREATEHTMLRESULT_H
#define CREATEHTMLRESULT_H

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QRunnable>
#include <QTextStream>

#include "satellite/calculation.h"
#include "satellite/ephemeris_container.h"

class CreateHtmlTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    CreateHtmlTask(EphemerisContainer *calculatedEphemeris,
                   EphemerisContainer *readedEphemeris = nullptr,
                   QObject *parent = nullptr);

    void run() override;

    QString html() const;

    int countOfProccessing() const;
    EphemerisContainer *calculatedEphemeris() const;
    EphemerisContainer *readedEphemeris() const;

signals:
    void finished();

private:
    QString _html;
    int _countOfProccessing = 0;
    EphemerisContainer *_calculatedEphemeris;
    EphemerisContainer *_readedEphemeris;
};

#endif // CREATEHTMLRESULT_H
