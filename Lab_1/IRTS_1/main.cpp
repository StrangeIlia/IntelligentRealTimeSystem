#include "main_window.h"
#include "result_form.h"

#include "parser_ephemeris.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
