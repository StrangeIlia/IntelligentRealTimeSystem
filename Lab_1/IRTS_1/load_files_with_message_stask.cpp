#include "load_files_with_message_stask.h"

LoadFilesWithMessagesTask::LoadFilesWithMessagesTask(QString fileName, QObject *parent) : QObject(parent) {
    setAutoDelete(false);
    _listFileNames.push_back(fileName);
}

LoadFilesWithMessagesTask::LoadFilesWithMessagesTask(QStringList listFileNames, QObject *parent) : QObject(parent)  {
    setAutoDelete(false);
    _listFileNames = listFileNames;
}

void LoadFilesWithMessagesTask::run() {
    ParserOfNavigationMessages parser;
    for(QString fileName : _listFileNames) {
        QFile file(fileName);
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        try {
            auto result = parser.convertStream(stream);
            for(auto message : result) {
                _messages.push_back(message);
            }
        } catch (std::exception) {
            break;
        }
        ++_countReadedFiles;
        file.close();
    }
    emit readFinished();
}

bool LoadFilesWithMessagesTask::ok() const {
    return _countReadedFiles == _listFileNames.size();
}

int LoadFilesWithMessagesTask::countReadedFiles() const {
    return _countReadedFiles;
}

int LoadFilesWithMessagesTask::countReadedMessages() const {
    return  _messages.count();
}

QStringList LoadFilesWithMessagesTask::listFileNames() const {
    return _listFileNames;
}

QVector<NavigationMessage*> LoadFilesWithMessagesTask::messages() const {
    return _messages;
}
