#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("操作");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    this->close();
}
