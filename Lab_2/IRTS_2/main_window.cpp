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

    connect(ui->addReciever, &QPushButton::clicked, this, &MainWindow::loadFilesWithObservations);
    connect(ui->calcPositionReciever, &QPushButton::clicked, this, &MainWindow::calculateRecieverPosition);
}

MainWindow::~MainWindow() {
    delete ui;
    delete messagesContainer;
    delete ephemerisContainer;
    for(auto rec : recievers) {
        delete rec;
    }
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
    printString(createHtmlTask->html());

    timer->deleteLater();
    dialog->deleteLater();
    calculateTask->deleteLater();
    createHtmlTask->deleteLater();
}

void MainWindow::calculateSelectedDateTime(bool /*ignored*/) {
    QDateTime datetime = ui->dateTimeEdit->dateTime();
    SatellitePositionCalculator calculator(messagesContainer);
    int satelliteNumber = ui->satelliteNumber->value();
    auto data = calculator.calculate(satelliteNumber, datetime);
    if(data == nullptr) {
        QString str = tr("Не удалось просчитать положения спутника №");
        str += QString::number(satelliteNumber);
        str += tr(" для даты ") + datetime.date().toString("dd.MM.yyyy");
        str += tr(" и времени ") + datetime.time().toString("HH.mm.ss");
        printString(str);
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


    printString(createHtmlTask->html());

    delete data;
    delete container;
    timer->deleteLater();
    dialog->deleteLater();
    createHtmlTask->deleteLater();
}

void MainWindow::loadFilesWithObservations(bool /*ignored*/) {
    ParserOfObservationData parser;

    QStringList listFileNames = QFileDialog::getOpenFileNames(this, tr("Выберите файл(-ы) для чтения"));
    for(QString fileName : listFileNames) {
        QFile file(fileName);
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        try {
            auto data = parser.convertStream(stream);
            data->recieverName = QFileInfo(file).fileName().left(4);
            addReciever(data);
        }  catch (...) {
            QErrorMessage *message = new QErrorMessage(this);
            message->setWindowTitle(tr("Ошибка при выполнении"));
            message->showMessage(tr("Ошибка при чтении файла \"") + fileName + "\"", tr("Чтение файла"));
        }
        file.close();
    }
}

void MainWindow::calculateRecieverPosition(bool /*ignored*/) {
    QString reciever = ui->recievers->currentText();
    RecieverData *data = nullptr;
    for(auto d : recievers) {
        if(d->recieverName == reciever) {
            data = d;
            break;
        }
    }

    RecieverPositionCalculator calculator;
    auto position = calculator.calculate(data, messagesContainer);

    if(position == nullptr) {
        printString("Не удалось найти координаты принимающей станции");
    } else {
        QString buffer;
        QTextStream stream(&buffer);
        stream.setRealNumberNotation(QTextStream::FixedNotation);
        stream.setRealNumberPrecision(5);
        stream << tr("Имя станции: ") << data->recieverName << "<br>";
        stream << "<table border=\"1\" cellpadding=\"5\">";

        stream << "<tr>";
        stream << "<td colspan=\"4\" style=\"text-align: center;\">" << tr("Вычисленные значения") << "</td>";
        stream << "<td colspan=\"3\" style=\"text-align: center;\">" << tr("Реальные значения") << "</td>";
        stream << "<td colspan=\"3\" style=\"text-align: center;\">" << tr("Разница в показаниях") << "</td>";
        stream << "</tr>";

        stream << "<tr>";
        stream << "<td style=\"text-align: center;\">X</td>";
        stream << "<td style=\"text-align: center;\">Y</td>";
        stream << "<td style=\"text-align: center;\">Z</td>";
        stream << "<td style=\"text-align: center;\">dT</td>";
        stream << "<td style=\"text-align: center;\">X</td>";
        stream << "<td style=\"text-align: center;\">Y</td>";
        stream << "<td style=\"text-align: center;\">Z</td>";
        stream << "<td style=\"text-align: center;\">dX</td>";
        stream << "<td style=\"text-align: center;\">dY</td>";
        stream << "<td style=\"text-align: center;\">dZ</td>";
        stream << "</tr>";

        stream << "<tr>";
        stream << "<td>" << position->x << "</td>";
        stream << "<td>" << position->y << "</td>";
        stream << "<td>" << position->z << "</td>";
        stream << "<td>" << position->dt << "</td>";
        stream << "<td>" << data->X << "</td>";
        stream << "<td>" << data->Y << "</td>";
        stream << "<td>" << data->Z << "</td>";
        stream << "<td>" << std::abs(position->x - data->X) << "</td>";
        stream << "<td>" << std::abs(position->y - data->Y) << "</td>";
        stream << "<td>" << std::abs(position->z - data->Z) << "</td>";
        stream << "</tr>";

        stream << "</table>";

        printString(buffer);

        delete position;
    }
}

void MainWindow::printString(QString html) {
    QString lastHtml;
    if(ui->console->document()->toPlainText() != "")
        lastHtml += ui->console->document()->toHtml() + "<br><br>";
    lastHtml += "-----------------------------------<br>";
    lastHtml += html;
    ui->console->document()->setHtml(lastHtml);
}

void MainWindow::clearConsole(bool /*ignored*/) {
    ui->console->document()->setHtml("");
}

void MainWindow::addReciever(RecieverData *data) {
    RecieverData *oldData = nullptr;
    for(auto d : recievers) {
        if(d->recieverName == data->recieverName) {
            oldData = d;
            break;
        }
    }
    recievers.append(data);
    if(oldData != nullptr) {
        recievers.removeAll(oldData);
    } else {
        ui->recievers->addItem(data->recieverName);
    }
}
