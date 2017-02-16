#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("设置");
    ui->zoomIn->setText("0.5");
    ui->zoomOut->setText("0.5");
    ui->qrcodeHeight->setText("200");
    ui->qrcodeWidth->setText("200");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    this->close();
    QString str = ui->zoomIn->text();
    if (str != "") {
        bool ok;
        double num = str.toDouble(&ok);
        if (ok && num > 0 && num <= 1) {
            emit sendZoomIn(num);
        }
    }
    str = ui->zoomOut->text();
    if (str != "") {
        bool ok;
        double num = str.toDouble(&ok);
        if (ok && num > 0 && num <= 1) {
            emit sendZoomOut(num);
        }
    }
    str = ui->qrcodeHeight->text();
    if (str != "") {
        bool ok;
        int num = str.toInt(&ok);
        if (ok) {
            emit sendHeight(num);
        }
    }
    str = ui->qrcodeWidth->text();
    if (str != "") {
        bool ok;
        int num = str.toInt(&ok);
        if (ok) {
            emit sendWidth(num);
        }
    }
}

void Dialog::on_pushButton_2_clicked()
{
    this->close();
}
