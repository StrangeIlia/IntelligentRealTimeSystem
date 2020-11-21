#include "dialog_processing.h"
#include "ui_dialog_processing.h"

DialogProcessing::DialogProcessing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProcessing)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->progressBar->setEnabled(false);
    this->_baseTitle = tr("Выполнение");
    setWindowTitle(this->_baseTitle);
    startTimer(1000);
}

DialogProcessing::DialogProcessing(QString baseTitle, QWidget *parent)  :
    QDialog(parent),
    ui(new Ui::DialogProcessing)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->progressBar->setEnabled(false);
    this->_baseTitle = baseTitle;
    setWindowTitle(this->_baseTitle);
    startTimer(1000);
}

DialogProcessing::~DialogProcessing()
{
    delete ui;
}


QString DialogProcessing::process() const {
    return ui->process->text();
}

QString DialogProcessing::value() const {
    return ui->value->text();
}

double DialogProcessing::persent() const {
    return 1. * (ui->progressBar->value() - ui->progressBar->minimum())/ ui->progressBar->maximum();
}

bool DialogProcessing::progressBarEnabled() const {
    return ui->progressBar->isEnabled();
}

QString DialogProcessing::baseTitle() const {
    return _baseTitle;
}

void DialogProcessing::setProcess(QString str) {
    ui->process->setText(str);
}

void DialogProcessing::setValue(QString str) {
    ui->value->setText(str);
}

void DialogProcessing::setPersent(double value) {
    if(value > 1) value = 1;
    if(value < 0) value = 0;
    ui->progressBar->setValue(value * ui->progressBar->maximum() + ui->progressBar->minimum());
}

void DialogProcessing::setEnableProgressBar(bool value) {
    ui->progressBar->setVisible(value);
    ui->progressBar->setEnabled(value);
}

void DialogProcessing::setBaseTitle(QString title) {
    _baseTitle = title;
}

void DialogProcessing::timerEvent(QTimerEvent *) {
    ++dotCount;
    if(dotCount >= 3) dotCount = 0;
    QString str;
    for(int i = 0; i != dotCount; ++i)
        str += ".";
    setWindowTitle(this->_baseTitle + str);
}
