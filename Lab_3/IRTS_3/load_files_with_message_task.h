#ifndef LOADFILESWITHMESSAGESTASK_H
#define LOADFILESWITHMESSAGESTASK_H

#include <QFile>
#include <QRunnable>
#include <QStringList>

#include "parser_of_navigation_messages.h"

class LoadFilesWithMessagesTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LoadFilesWithMessagesTask(QString fileName, QObject *parent = nullptr);
    LoadFilesWithMessagesTask(QStringList listFileNames, QObject *parent = nullptr);

    void run() override;

    bool ok() const;

    int countReadedFiles() const;
    int countReadedMessages() const;
    QStringList listFileNames() const;
    QVector<NavigationMessage*> messages() const;

signals:
    void readFinished();

private:
    QStringList _listFileNames;
    int _countReadedFiles = 0;
    QVector<NavigationMessage*> _messages;
};

#endif // LOADFILESWITHMESSAGESTASK_H
