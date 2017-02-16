#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
signals:
    sendZoomIn(double a);
    sendZoomOut(double a);
    sendHeight(int a);
    sendWidth(int a);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
