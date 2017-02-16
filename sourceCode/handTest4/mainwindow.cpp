#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QButtonGroup>
#include <QGraphicsScene>
#include <QDebug>
#include <imageitem.h>
#include <QFileDialog>
#include <QInputDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPointF>
#include <QTransform>
#include <textitem.h>
#include <QFont>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QMessageBox>
#include <QByteArray>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>
#include <QRectF>
#include <QScrollBar>
#include <QIcon>
#include "qrcode/qrencode.h"
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("手持编辑软件"));
    myScene = new QGraphicsScene();
    myScene->installEventFilter(this);

    //为菜单栏加动作
    setAction = new QAction("设置");
    aboutAction = new QAction("操作");
    ui->menu->addAction(setAction);
    ui->menu_2->addAction(aboutAction);

    //创建窗口并隐藏
    aboutDialog = new AboutDialog();
    aboutDialog->hide();
    setDialog = new Dialog();
    setDialog->hide();

    QRectF* rectf = new QRectF(0,0,800,470);
    ui->graphicsView->setScene(myScene);
    //设置禁用滑动条
    ui->graphicsView->horizontalScrollBar()->setVisible(false);
    ui->graphicsView->verticalScrollBar()->setVisible(false);
    ui->graphicsView->setSceneRect(*rectf);
    myScene->setSceneRect(*rectf);

    //设置字体样式和大小下拉框
    ui->fontComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    QFontDatabase db;
    foreach(int size,db.standardSizes())
        ui->sizeComboBox->addItem(QString::number(size));

    //将按钮分组
    group = new QButtonGroup();
    group->addButton(ui->pointerBtn,0);
    group->addButton(ui->imageBtn,1);
    group->addButton(ui->textBtn,2);
    group->addButton(ui->qrcodeBtn,3);

    //绑定信号和槽
    connect(ui->pointerBtn,SIGNAL(clicked(bool)),this,SLOT(setLoadType()));
    connect(ui->imageBtn,SIGNAL(clicked(bool)),this,SLOT(setLoadType()));
    connect(ui->textBtn,SIGNAL(clicked(bool)),this,SLOT(setLoadType()));
    connect(ui->qrcodeBtn,SIGNAL(clicked(bool)),this,SLOT(setLoadType()));

    connect(ui->fontComboBox,SIGNAL(activated(QString)),this,SLOT(showFontComboBox(QString)));
    connect(ui->sizeComboBox,SIGNAL(activated(QString)),this,SLOT(showSizeComboBox(QString)));

    connect(setAction,SIGNAL(triggered(bool)),this,SLOT(showSetDialog()));
    connect(aboutAction,SIGNAL(triggered(bool)),this,SLOT(showAboutDialog()));

    //设置各种参数绑定的槽函数
    connect(setDialog,SIGNAL(sendZoomIn(double)),this,SLOT(setZoomInNum(double)));
    connect(setDialog,SIGNAL(sendZoomOut(double)),this,SLOT(setZoomOutNum(double)));
    connect(setDialog,SIGNAL(sendHeight(int)),this,SLOT(setQRcodeHeight(int)));
    connect(setDialog,SIGNAL(sendWidth(int)),this,SLOT(setQRcodeWidth(int)));

    //未解之谜，必须先加上一个TextItem然后再去掉，拖动图片才不会发生跳动事件
    textItem= new TextItem("text");
    myScene->addItem(textItem);
    myScene->removeItem(textItem);

    //设置窗口的大小不能发生改变
    this->setFixedSize(830,660);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setLoadType()
{
    switch (group->checkedId()) {
    case 0:
        loadType = 0;
        break;
    case 1:
        loadType = 1;
        break;
    case 2:
        loadType = 2;
        break;
    case 3:
        loadType = 3;
        break;
    default:
        break;
    }
}
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == myScene) {
        if (event->type() == QEvent::GraphicsSceneMousePress) {
            QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            loadFile(mouseEvent);
            return false;
        } else if(event->type() == QEvent::GraphicsSceneMouseMove) {
            QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            showLoc();
            return false;
        } else {
            return false;
        }
    } else {
        return MainWindow::eventFilter(watched, event);
    }
}
void MainWindow::loadFile(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "loadType" << loadType;
    //点的位置
    if(loadType == POINTER) {
        list = myScene->items();
        //实时显示选中元素的位置
        foreach (QGraphicsItem *item, list) {
            if(item->isSelected()) {
                QPointF point = item->scenePos();
                QString x = QString::number(point.x());
                QString y = QString::number(point.y());
                ui->lineEdit_x->setText(x);
                ui->lineEdit_y->setText(y);
                textItem = dynamic_cast<TextItem*>(item);
                if (textItem != NULL) {
                    QFont font = textItem->font();
                    ui->fontComboBox->setCurrentFont(font);
                    int size = font.pointSize();
                    ui->sizeComboBox->setCurrentText(QString::number(size));
                }
            }
        }
    } else if (loadType == LOAD_IMAGE) {
        QString fileName = QFileDialog::getOpenFileName(this,"Open Image","",tr("Image files(*.png *.jpg *.bmp)"));
        if(fileName.isEmpty())
            return;
        QImage *image = new QImage(fileName);
        QPixmap pixmap = QPixmap::fromImage(*image);
        imageItem = new ImageItem();
        imageItem->setPixmap(pixmap);
        myScene->addItem(imageItem);
        QPointF point = mouseEvent->scenePos();
        imageItem->setPos(point.x(),point.y());
    } else if(loadType == LOAD_TEXT) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("输入文本"),
                                                tr("Text"), QLineEdit::Normal,
                                                        QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty()) {
            textItem= new TextItem(text);
            myScene->addItem(textItem);
            QPointF point = mouseEvent->scenePos();
            textItem->setPos(point.x(),point.y());
        }
    } else if (loadType == LOAD_QRCODE) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("输入二维码内容"),
                                                tr("Text"), QLineEdit::Normal,
                                                        "", &ok);
        if (ok && !text.isEmpty()) {
            QRcode *qrcode;
            qrcode = QRcode_encodeString(text.toStdString().c_str(),2,QR_ECLEVEL_Q,QR_MODE_8,1);
            qint32 temp_width = qrcodeWidth;
            qint32 temp_height = qrcodeHeight;
            qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
            double scale_x = (double)temp_width / (double)qrcode_width;
            double scale_y = (double)temp_height / (double)qrcode_width;
            QImage mainimg = QImage(temp_width,temp_height,QImage::Format_ARGB32);
            QPainter painter(&mainimg);
            QColor background(Qt::white);
            painter.setBrush(background);
            painter.setPen(Qt::NoPen);
            painter.drawRect(0,0,temp_width,temp_height);
            QColor foreground(Qt::black);
            painter.setBrush(foreground);
            for (qint32 y=0; y<qrcode_width; y++) {
                for (qint32 x=0; x<qrcode_width; x++) {
                    unsigned char b = qrcode->data[y*qrcode_width+x];
                    if(b & 0x01) {
                        QRectF r(x*scale_x,y*scale_y,scale_x,scale_y);
                        painter.drawRects(&r,1);
                    }
                }
            }
            QPixmap pixmap=QPixmap::fromImage(mainimg);
            imageItem = new ImageItem();
            imageItem->setPixmap(pixmap);
            myScene->addItem(imageItem);
            QPointF point = mouseEvent->scenePos();
            imageItem->setPos(point.x(),point.y());
        }
    }
    setCheckPointer();
}
void MainWindow::setCheckPointer()
{
    ui->pointerBtn->setChecked(true);
    loadType = 0;
}
void MainWindow::on_deleteBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            myScene->removeItem(item);
        }
    }
}

void MainWindow::on_zoomInBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            item->setScale(item->scale()+zoomInNum);
        }
    }
}

void MainWindow::on_zoomOutBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            qreal temp = item->scale() - zoomOutNum;
            if(temp > 0)
                item->setScale(item->scale()-zoomOutNum);
        }
    }
}
void MainWindow::showLoc()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            QPointF point = item->scenePos();
            QString x = QString::number(point.x());
            QString y = QString::number(point.y());
            ui->lineEdit_x->setText(x);
            ui->lineEdit_y->setText(y);
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                bool flag = font.bold();
                ui->boldBtn->setChecked(flag);
                flag = font.italic();
                ui->italicBtn->setChecked(flag);
                flag = font.underline();
                ui->underlineBtn->setChecked(flag);
            }
        }
    }
}

void MainWindow::on_boldBtn_clicked(bool checked)
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                font.setBold(checked);
                textItem->setFont(font);
            }
        }
    }
}

void MainWindow::on_italicBtn_clicked(bool checked)
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                font.setItalic(checked);
                textItem->setFont(font);
            }
        }
    }
}

void MainWindow::on_underlineBtn_clicked(bool checked)
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                font.setUnderline(checked);
                textItem->setFont(font);
            }
        }
    }
}
void MainWindow::showFontComboBox(QString str)
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                font.setFamily(str);
                textItem->setFont(font);
            }
        }
    }
}
void MainWindow::showSizeComboBox(QString str)
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if(textItem != NULL) {
                QFont font = textItem->font();
                bool ok;
                int num = str.toInt(&ok);
                font.setPointSize(num);
                textItem->setFont(font);
            }
        }
    }
}

void MainWindow::on_addxBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            QPointF point = item->scenePos();
            QPointF later = QPointF(point.x()+1,point.y());
            item->setPos(later);
            QString x = QString::number(point.x()+1);
            ui->lineEdit_x->setText(x);
        }
    }
}

void MainWindow::on_reducexBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if(item->isSelected()) {
            QPointF point = item->scenePos();
            QPointF later = QPointF(point.x()-1,point.y());
            item->setPos(later);
            QString x = QString::number(point.x()-1);
            ui->lineEdit_x->setText(x);
        }
    }
}

void MainWindow::on_addyBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if (item->isSelected()) {
            QPointF point = item->scenePos();
            QPointF later = QPointF(point.x(),point.y()+1);
            item->setPos(later);
            QString y = QString::number(point.y()+1);
            ui->lineEdit_y->setText(y);
        }
    }
}

void MainWindow::on_reduceyBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if (item->isSelected()) {
            QPointF point = item->scenePos();
            QPointF later = QPointF(point.x(),point.y()-1);
            item->setPos(later);
            QString y = QString::number(point.y()-1);
            ui->lineEdit_y->setText(y);
        }
    }
}

void MainWindow::on_saveBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"save file","","XML File(*.xml)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("xmlfile");
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        imageItem = dynamic_cast<ImageItem*>(item);
        if (imageItem != NULL) {
            QPixmap pixmap = imageItem->pixmap();
            QImage image = pixmap.toImage();
            QByteArray ba;
            QBuffer buffer(&ba);
            if (!buffer.open(QIODevice::WriteOnly)) {
                QMessageBox::information(this,tr("Save Xml"),tr("Save Xml Failed"));
            }
            //必须指明保存格式
            image.save(&buffer,"BMP");
            buffer.close();
            QByteArray array = ba.toHex();
            QString temp = array;
            QPointF point = imageItem->scenePos();
            QString x = QString::number(point.x());
            QString y = QString::number(point.y());
            stream.writeStartElement("item");
            stream.writeAttribute("typename","image");
            stream.writeTextElement("value",temp);
            stream.writeTextElement("x",x);
            stream.writeTextElement("y",y);
            stream.writeEndElement();
        } else {
            textItem = dynamic_cast<TextItem*>(item);
            if (textItem != NULL) {
                QPointF point = textItem->scenePos();
                QString x = QString::number(point.x());
                QString y = QString::number(point.y());
                QString text = textItem->toPlainText();
                QFont font = textItem->font();
                QString fontFamily = font.family();
                QString size = QString::number(font.pointSize());
                stream.writeStartElement("item");
                stream.writeAttribute("typename","text");
                stream.writeTextElement("value",text);
                stream.writeTextElement("x",x);
                stream.writeTextElement("y",y);
                if (font.bold()) {
                    stream.writeTextElement("bold","1");
                } else {
                    stream.writeTextElement("bold","0");
                }
                if (font.italic()) {
                    stream.writeTextElement("italic","1");
                } else {
                    stream.writeTextElement("italic","0");
                }
                if (font.underline()) {
                    stream.writeTextElement("underline","1");
                } else {
                    stream.writeTextElement("underline","0");
                }
                stream.writeTextElement("fontFamily",fontFamily);
                stream.writeTextElement("size",size);
                stream.writeEndElement();
            }
        }
    }
    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
}

void MainWindow::on_readBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        myScene->removeItem(item);
    }
    QString fileName = QFileDialog::getOpenFileName(this,"open file","","XML File(*.xml)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;
    reader.setDevice(&file);
    while (!reader.atEnd()) {
        // 读取下一个记号，它返回记号的类型
        QXmlStreamReader::TokenType type = reader.readNext();
        // 下面便根据记号的类型来进行不同的输出
        if (type == QXmlStreamReader::StartElement) {
            if (reader.name() == "item" && reader.attributes().hasAttribute("typename")) {
                if (reader.attributes().value("typename") == "image")
                    readImage();
                else if (reader.attributes().value("typename") == "text")
                    readText();
            }
        }
    }
    // 如果读取过程中出现错误，那么输出错误信息
    if (reader.hasError()) {
        qDebug() << "error: " << reader.errorString();
    }
    file.close();
}
void MainWindow::readImage()
{
    qDebug() << "readImage";
    QPixmap pixmap;
    QString x;
    QString y;
    while(!reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();
        if (type == QXmlStreamReader::StartElement) {
            if (reader.name() == "value") {
                QString str = reader.readElementText();
                pixmap.loadFromData(QByteArray::fromHex(str.toLatin1()));
            } else if (reader.name() == "x") {
                x = reader.readElementText();
            } else if (reader.name() == "y") {
                y = reader.readElementText();
            }
        } else if (type == QXmlStreamReader::EndElement && reader.name() == "item") {
            imageItem = new ImageItem();
            imageItem->setPixmap(pixmap);
            myScene->addItem(imageItem);
            imageItem->setPos(x.toInt(),y.toInt());
            break;
        }
    }
}
void MainWindow::readText()
{
    qDebug() << "readText";
    QString text;
    QString x;
    QString y;
    QString str;
    QFont font;
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType type = reader.readNext();
        if (type == QXmlStreamReader::StartElement) {
            if (reader.name() == "value") {
                text = reader.readElementText();
            } else if (reader.name() == "x") {
                x = reader.readElementText();
            } else if (reader.name() == "y") {
                y = reader.readElementText();
            } else if (reader.name() == "bold") {
                str = reader.readElementText();
                int num = str.toInt();
                num == 1 ? font.setBold(true) : font.setBold(false);
            } else if (reader.name() == "italic") {
                str = reader.readElementText();
                int num = str.toInt();
                num == 1 ? font.setItalic(true) : font.setItalic(false);
            } else if (reader.name() == "underline") {
                str = reader.readElementText();
                int num = str.toInt();
                num == 1 ? font.setUnderline(true) : font.setUnderline(false);
            } else if (reader.name() == "fontFamily") {
                str = reader.readElementText();
                font.setFamily(str);
            } else if (reader.name() == "size") {
                str = reader.readElementText();
                font.setPointSize(str.toInt());
            }
        } else if (type == QXmlStreamReader::EndElement && reader.name() == "item") {
            textItem = new TextItem(text);
            textItem->setFont(font);
            myScene->addItem(textItem);
            textItem->setPos(x.toInt(),y.toInt());
            break;
        }
    }
}

void MainWindow::on_saveImageBtn_clicked()
{
    QPixmap pixmap = QPixmap::grabWidget(ui->graphicsView);
    QImage image = pixmap.toImage();
    QString fileName = QFileDialog::getSaveFileName(this,"save file","","Image Files (*.png *.jpg *.bmp)");
    if (fileName.isEmpty())
        return;
    image.save(fileName);
}

void MainWindow::on_modifyBtn_clicked()
{
    list = myScene->items();
    foreach (QGraphicsItem *item, list) {
        if (item->isSelected()) {
            textItem = dynamic_cast<TextItem*>(item);
            if (textItem != NULL) {
                bool ok;
                QString text = textItem->toPlainText();
                QString str = QInputDialog::getText(this, tr("输入要修改的文本"),
                                                        tr("Text"), QLineEdit::Normal,
                                                         text, &ok);
                if (ok && !text.isEmpty()) {
                    textItem->setPlainText(str);
                }
            }
        }
    }
}
void MainWindow::showSetDialog()
{
    setDialog->show();
}
void MainWindow::showAboutDialog()
{
    aboutDialog->show();
}
void MainWindow::setZoomInNum(double a)
{
    zoomInNum = a;
}
void MainWindow::setZoomOutNum(double a)
{
    zoomOutNum = a;
}
void MainWindow::setQRcodeWidth(int w)
{
    qrcodeWidth = w;
}
void MainWindow::setQRcodeHeight(int h)
{
    qrcodeHeight = h;
}
