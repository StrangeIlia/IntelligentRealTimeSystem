#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , threadPool(new QThreadPool(this))
    , ephemerisContainer(new EphemerisContainer())
    , messagesContainer(new SatelliteMessagesContainer())
{
    ui->setupUi(this);
    threadPool->setMaxThreadCount(1);
    connect(ui->calcAll, &QPushButton::clicked, this, &MainWindow::calculateAllData);
    connect(ui->calcSelected, &QPushButton::clicked, this, &MainWindow::calculateSelectedDateTime);
    connect(ui->filesWithMessages, &QPushButton::clicked, this, &MainWindow::loadFilesWithMessages);
    connect(ui->filesWithEphemeris, &QPushButton::clicked, this, &MainWindow::loadFilesWithEphemeris);
}

MainWindow::~MainWindow() {
    delete ui;
    delete messagesContainer;
    delete ephemerisContainer;
}

void MainWindow::loadFilesWithMessages(bool /*ignored*/) {
    QStringList listFileNames = QFileDialog::getOpenFileNames(this, tr("Выберите файл(-ы) для чтения"));
    int fileCount = listFileNames.size();
    QTimer *timer = new QTimer(this);
    LoadFilesWithMessagesTask *task = new LoadFilesWithMessagesTask(listFileNames);
    DialogProcessing *dialog = new DialogProcessing(tr("Считывание файла"), this);
    connect(task, &LoadFilesWithMessagesTask::readFinished, dialog, &DialogProcessing::close);
    connect(timer, &QTimer::timeout, [fileCount, dialog, task]() {
        dialog->setValue(QString::number(task->countReadedFiles()) + "/" + QString::number(fileCount));
    });
    threadPool->start(task);
    timer->start(1000);
    dialog->exec();
    timer->stop();
    for(auto message : task->messages()) {
        messagesContainer->addMessage(message);
    }
    if(!task->ok()) {
        QErrorMessage *message = new QErrorMessage(this);
        message->setWindowTitle(tr("Ошибка при выполнении"));
        message->showMessage(tr("Ошибка при чтении файла \"") + listFileNames[task->countReadedFiles()] + "\"", tr("Чтение файла"));
    }
    task->deleteLater();
    timer->deleteLater();
    dialog->deleteLater();
}

void MainWindow::loadFilesWithEphemeris(bool /*ignored*/) {
    QStringList listFileNames = QFileDialog::getOpenFileNames(this, tr("Выберите файл(-ы) для чтения"));
    int fileCount = listFileNames.size();
    QTimer *timer = new QTimer(this);
    LoadFilesWithEphemerisTask *task = new LoadFilesWithEphemerisTask(listFileNames);
    DialogProcessing *dialog = new DialogProcessing(tr("Считывание файла"), this);
    connect(task, &LoadFilesWithEphemerisTask::readFinished, dialog, &DialogProcessing::close);
    connect(timer, &QTimer::timeout, [fileCount, dialog, task]() {
        dialog->setValue(QString::number(task->countReadedFiles()) + "/" + QString::number(fileCount));
    });
    threadPool->start(task);
    timer->start(1000);
    dialog->exec();
    timer->stop();
    for(auto ephemeris : task->ephemeris()) {
        ephemerisContainer->addEphemeris(ephemeris);
    }
    if(!task->ok()) {
        QErrorMessage *message = new QErrorMessage(this);
        message->setWindowTitle(tr("Ошибка при выполнении"));
        message->showMessage(tr("Ошибка при чтении файла \"") + listFileNames[task->countReadedFiles()] + "\"", tr("Чтение файла"));
    }
    task->deleteLater();
    timer->deleteLater();
    dialog->deleteLater();
}

void MainWindow::calculateAllData(bool /*ignored*/) {

}

void MainWindow::calculateSelectedDateTime(bool /*ignored*/) {

}

void MainWindow::clearConsole(bool /*ignored*/) {
    ui->result->document()->setHtml("");
}
