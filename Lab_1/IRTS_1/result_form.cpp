#include "result_form.h"
#include "ui_result_form.h"

ResultForm::ResultForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultForm)
{
    ui->setupUi(this);
}

ResultForm::~ResultForm()
{
    delete ui;
}

QTextDocument *ResultForm::doc() const {
    return ui->textEdit->document();
}
