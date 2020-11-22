#include "main_window.h"

#include <QApplication>

#include <QDebug>

#include "parser_of_observation_data.h"
#include "parser_of_navigation_messages.h"

#include "satellite/reciever_position_calculator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    RecieverData *recieverData;
//    {
//        QFile file("gls10550.16o");
//        QFile file("7odm0550.16o");
//        file.open(QFile::OpenModeFlag::ReadOnly);
//        QTextStream stream(&file);
//        ParserOfObservationData parser;
//        recieverData = parser.convertStream(stream);
//    }

//    SatelliteMessagesContainer *container = new SatelliteMessagesContainer();
//    {
//        QFile file("gls10550.16n");
//        QFile file("7odm0550.16n");
//        file.open(QFile::OpenModeFlag::ReadOnly);
//        QTextStream stream(&file);
//        ParserOfNavigationMessages parser;
//        auto messages = parser.convertStream(stream);

//        for(auto message : messages) {
//            container->addMessage(message);
//        }
//    }

//    RecieverPositionCalculator calculator;
//    auto position = calculator.calculate(recieverData, container);

//    QString buffer;
//    QTextStream stream(&buffer);
//    stream.setRealNumberNotation(QTextStream::FixedNotation);
//    stream << position->x << " " << position->y << " " << position->z;
//    qDebug() << buffer; buffer.clear();

//    stream << recieverData->X << " " << recieverData->Y << " " << recieverData->Z;
//    qDebug() << buffer; buffer.clear();

//    double dx = std::abs(recieverData->X - position->x);
//    double dy = std::abs(recieverData->Y - position->y);
//    double dz = std::abs(recieverData->Z - position->z);

//    stream << dx << " " << dy << " " << dz;
//    qDebug() << buffer; buffer.clear();

//    for(auto &obs : recieverData->observations) {
//        auto position = calculator.calculate(obs, container);

//        QString buffer;
//        QTextStream stream(&buffer);
//        stream.setRealNumberNotation(QTextStream::FixedNotation);
//        stream << position->x << " " << position->y << " " << position->z;
//        qDebug() << buffer; buffer.clear();

//        stream << recieverData->X << " " << recieverData->Y << " " << recieverData->Z;
//        qDebug() << buffer; buffer.clear();
//        qDebug() << "";
//    }

//    delete recieverData;
//    delete container;

    return a.exec();
}
