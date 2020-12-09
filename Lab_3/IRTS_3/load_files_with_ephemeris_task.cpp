#include "load_files_with_ephemeris_task.h"

LoadFilesWithEphemerisTask::LoadFilesWithEphemerisTask(QString fileName, QObject *parent) : QObject(parent) {
    setAutoDelete(false);
    _listFileNames.push_back(fileName);
}

LoadFilesWithEphemerisTask::LoadFilesWithEphemerisTask(QStringList listFileNames, QObject *parent) : QObject(parent) {
    setAutoDelete(false);
    _listFileNames = listFileNames;
}

void LoadFilesWithEphemerisTask::run() {
    ParserEphemeris parser;
    for(QString fileName : _listFileNames) {
        QFile file(fileName);
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        try {
            auto result = parser.convertStream(stream);
            for(auto ephemeris : result) {
                _ephemeris.push_back(ephemeris);
            }
        } catch (std::exception) {
            break;
        }
        ++_countReadedFiles;
        file.close();
    }
    emit readFinished();
}

bool LoadFilesWithEphemerisTask::ok() const {
    return _countReadedFiles == _listFileNames.size();
}

int LoadFilesWithEphemerisTask::countReadedFiles() const {
    return _countReadedFiles;
}

int LoadFilesWithEphemerisTask::countReadedEphemeris() const {
    return  _ephemeris.count();
}

QStringList LoadFilesWithEphemerisTask::listFileNames() const {
    return _listFileNames;
}

QVector<Ephemeris*> LoadFilesWithEphemerisTask::ephemeris() const {
    return _ephemeris;
}
