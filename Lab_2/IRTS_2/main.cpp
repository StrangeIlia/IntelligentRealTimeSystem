#include "main_window.h"

#include <QApplication>

#include <QDebug>
#include "parser_of_observation_data.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    QFile file("gls10550.16o");
//    file.open(QFile::OpenModeFlag::ReadOnly);
//    QTextStream stream(&file);
//    ParserOfObservationData parser;
//    auto vector = parser.convertStream(stream);

//    auto data = vector.first();
//    qDebug() << data->dateTime;
//    for(auto iter = data->gps.begin(); iter != data->gps.end(); ++iter) {
//        QString buffer;
//        QTextStream tmpStream(&buffer);
//        tmpStream << iter.key() << " " << iter.value().c1;
//        qDebug() << buffer;
//    }

//    for(auto data : vector) delete data;

    return a.exec();
}
