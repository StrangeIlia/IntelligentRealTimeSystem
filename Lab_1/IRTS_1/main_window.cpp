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
    connect(ui->clearConsole, &QPushButton::clicked, this, &MainWindow::clearConsole);
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
    dialog->setProcess(tr("Всего считано файлов: "));
    connect(task, &LoadFilesWithMessagesTask::readFinished, dialog, &DialogProcessing::close);
    connect(timer, &QTimer::timeout, [fileCount, dialog, task]() {
        dialog->setValue(QString::number(task->countReadedFiles()) + "/" + QString::number(fileCount));
    });
    threadPool->start(task);
    timer->start(20);
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
    dialog->setProcess(tr("Всего считано файлов: "));
    connect(task, &LoadFilesWithEphemerisTask::readFinished, dialog, &DialogProcessing::close);
    connect(timer, &QTimer::timeout, [fileCount, dialog, task]() {
        dialog->setValue(QString::number(task->countReadedFiles()) + "/" + QString::number(fileCount));
    });
    threadPool->start(task);
    timer->start(20);
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
    int countMessages = messagesContainer->messageCount();
    QTimer *timer = new QTimer(this);
    CalculateTask *calculateTask = new CalculateTask(messagesContainer);
    DialogProcessing *dialog = new DialogProcessing(tr("Расчет положений спутников"), this);
    dialog->setEnableProgressBar(true);
    dialog->setProcess(tr("Задача выполняется: "));
    connect(calculateTask, &CalculateTask::finished, dialog, &DialogProcessing::close);
    auto connection = connect(timer, &QTimer::timeout, [countMessages, dialog, calculateTask]() {
        dialog->setPersent(1.0 * calculateTask->amountOfProcessedData() / countMessages);
    });
    threadPool->start(calculateTask);
    timer->start(20);
    dialog->exec();
    timer->stop();
    disconnect(connection);
    disconnect(calculateTask, &CalculateTask::finished, dialog, &DialogProcessing::close);
    // ---------------------------------------------
    CreateHtmlTask *createHtmlTask = new CreateHtmlTask(calculateTask->results(), ephemerisContainer);
    int countEphemeris = calculateTask->results()->ephemerisCount();
    dialog->setBaseTitle(tr("Подготовка данных к выводу"));
    connect(createHtmlTask, &CreateHtmlTask::finished, dialog, &DialogProcessing::close);
    connection = connect(timer, &QTimer::timeout, [countEphemeris, dialog, createHtmlTask]() {
        dialog->setPersent(1.0 * createHtmlTask->countOfProccessing() / countEphemeris);
    });
    threadPool->start(createHtmlTask);
    timer->start(20);
    dialog->exec();
    timer->stop();
    disconnect(connection);

    delete calculateTask->results();
    auto lastHtml = ui->console->document()->toHtml();
    ui->console->document()->setHtml(lastHtml + createHtmlTask->html());

    timer->deleteLater();
    dialog->deleteLater();
    calculateTask->deleteLater();
    createHtmlTask->deleteLater();
}

void MainWindow::calculateSelectedDateTime(bool /*ignored*/) {
    auto lastHtml = ui->console->document()->toHtml();
    QDateTime datetime = ui->dateTimeEdit->dateTime();
    PositionCalculator calculator(messagesContainer);
    int satelliteNumber = ui->satelliteNumber->value();
    auto data = calculator.calculate(satelliteNumber, datetime);
    if(data == nullptr) {
        QString str = "<br><br>-----------------------------------<br>";
        str += tr("Не удалось просчитать положения спутника №");
        str += QString::number(satelliteNumber);
        str += tr(" для даты ") + datetime.date().toString("dd.MM.yyyy");
        str += tr(" и времени ") + datetime.time().toString("HH.mm.ss");
        ui->console->document()->setHtml(lastHtml + str);
        return;
    }

    Ephemeris *ephemeris = new Ephemeris();
    ephemeris->dateTime = datetime;
    SatelliteInfo info;
    info.x = data->satellitePosX;
    info.y = data->satellitePosY;
    info.z = data->satellitePosZ;
    ephemeris->satelliteInfo[satelliteNumber] = info;
    EphemerisContainer *container = new EphemerisContainer();
    container->addEphemeris(ephemeris);

    QTimer *timer = new QTimer(this);
    DialogProcessing *dialog = new DialogProcessing(tr("Подготовка данных к выводу"), this);
    CreateHtmlTask *createHtmlTask = new CreateHtmlTask(container, ephemerisContainer);
    int countEphemeris = 1;
    connect(createHtmlTask, &CreateHtmlTask::finished, dialog, &DialogProcessing::close);
    auto connection = connect(timer, &QTimer::timeout, [countEphemeris, dialog, createHtmlTask]() {
        dialog->setPersent(1.0 * createHtmlTask->countOfProccessing() / countEphemeris);
    });
    threadPool->start(createHtmlTask);
    timer->start(20);
    dialog->exec();
    timer->stop();
    disconnect(connection);


    ui->console->document()->setHtml(lastHtml + createHtmlTask->html());

    delete data;
    delete container;
    timer->deleteLater();
    dialog->deleteLater();
    createHtmlTask->deleteLater();
}

void MainWindow::clearConsole(bool /*ignored*/) {
    ui->console->document()->setHtml("");
}
