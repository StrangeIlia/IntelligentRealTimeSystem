#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QThreadPool>
#include <QErrorMessage>

#include "calculate_task.h"
#include "create_html_task.h"
#include "dialog_processing.h"
#include "load_files_with_ephemeris_task.h"
#include "load_files_with_message_stask.h"
#include "satellite/position_calculator.h"
#include "satellite/ephemeris_container.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadFilesWithMessages(bool ignored);
    void loadFilesWithEphemeris(bool ignored);
    void calculateAllData(bool ignored);
    void calculateSelectedDateTime(bool ignored);
    void clearConsole(bool ignored);

private:
    Ui::MainWindow *ui;
    QThreadPool *threadPool;
    EphemerisContainer *ephemerisContainer;
    SatelliteMessagesContainer *messagesContainer;
};
#endif // MAINWINDOW_H
