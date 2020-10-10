#include "main_window.h"
#include "result_form.h"

#include "parser_ephemeris.h"
#include "parser_of_navigation_messages.h"
#include "satellite/position_calculator.h"

#include <QFile>
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    PositionCalculator calculator;

    QFile fileMessage("gls10550.16n");
    fileMessage.open(QFile::ReadOnly);
    QTextStream messageStream(&fileMessage);
    ParserOfNavigationMessages parserMessages;
    QVector<NavigationMessage*> messages = parserMessages.convertStream(messageStream);



    return a.exec();
}
