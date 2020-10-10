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
    MainWindow w;
    w.show();

    return a.exec();
}
