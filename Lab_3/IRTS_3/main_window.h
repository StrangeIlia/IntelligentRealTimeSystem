#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QDateTime>
#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QThreadPool>
#include <QErrorMessage>

#include "calculate_task.h"
#include "create_html_task.h"
#include "dialog_processing.h"
#include "load_files_with_ephemeris_task.h"
#include "load_files_with_message_task.h"
#include "satellite/satellite_position_calculator.h"
#include "satellite/ephemeris_container.h"

#include "parser_of_observation_data.h"
#include "satellite/reciever_position_calculator.h"

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

    void loadFilesWithObservations(bool ignored);
    void calculateRecieverPosition(bool ignored);
    void calculateRecieverPositionDiff(bool ignored);

    void printString(QString html);
    void clearConsole(bool ignored);

private:    
    Ui::MainWindow *ui;
    QThreadPool *threadPool;
    EphemerisContainer *ephemerisContainer;
    SatelliteMessagesContainer *messagesContainer;   

    QVector<RecieverData*> recievers;

    void addReciever(RecieverData *data);
};
#endif // MAINWINDOW_H
