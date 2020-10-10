#ifndef LOADFILESWITHEPHEMERIS_H
#define LOADFILESWITHEPHEMERIS_H

#include <QFile>
#include <QRunnable>
#include <QStringList>

#include "parser_ephemeris.h"

class LoadFilesWithEphemerisTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadFilesWithEphemerisTask(QString fileName);
    LoadFilesWithEphemerisTask(QStringList listFileNames);

    void run() override;

    int countReadedFiles() const;
    int countReadedEphemeris() const;
    QStringList listFileNames() const;
    QVector<Ephemeris*> ephemeris() const;

signals:
    void readFinished();

private:
    QStringList _listFileNames;
    int _countReadedFiles = 0;
    QVector<Ephemeris*> _ephemeris;
};

#endif // LOADFILESWITHEPHEMERIS_H
