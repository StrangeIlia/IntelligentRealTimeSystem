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

    return a.exec();
}
