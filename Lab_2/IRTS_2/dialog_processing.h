#ifndef DIALOG_PROCESSING_H
#define DIALOG_PROCESSING_H

#include <QTimer>
#include <QDialog>

namespace Ui {
class DialogProcessing;
}

class DialogProcessing : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProcessing(QWidget *parent = nullptr);
    explicit DialogProcessing(QString _baseTitle, QWidget *parent = nullptr);
    ~DialogProcessing();

    QString process() const;
    QString value() const;

    double persent() const;
    bool progressBarEnabled() const;

    QString baseTitle() const;

public slots:
    void setProcess(QString str);
    void setValue(QString str);

    void setPersent(double value);
    void setEnableProgressBar(bool value);

    void setBaseTitle(QString title);

private:
    Ui::DialogProcessing *ui;

    QString _baseTitle;
    int dotCount = 0;

    void timerEvent(QTimerEvent *event) override;
};

#endif // DIALOG_PROCESSING_H
