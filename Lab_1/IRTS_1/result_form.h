#ifndef RESULT_FORM_H
#define RESULT_FORM_H

#include <QMainWindow>
#include <QTextDocument>

namespace Ui {
class ResultForm;
}

class ResultForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit ResultForm(QWidget *parent = nullptr);
    ~ResultForm();

    QTextDocument *doc() const;

private:
    Ui::ResultForm *ui;
};

#endif // RESULT_FORM_H
