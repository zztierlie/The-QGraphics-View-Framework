#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <imageitem.h>
#include <textitem.h>
#include <QList>
#include <QGraphicsItem>
#include <QXmlStreamReader>
#include <QAction>
#include <dialog.h>
#include <aboutdialog.h>

namespace Ui {
class MainWindow;
}

enum loadFile {
    POINTER,
    LOAD_IMAGE,
    LOAD_TEXT,
    LOAD_QRCODE,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QButtonGroup *group;
    int loadType;
    ImageItem *imageItem;
    TextItem *textItem;
    void setCheckPointer();
    QXmlStreamReader reader;
    QAction *setAction,*aboutAction;
    Dialog *setDialog;
    AboutDialog *aboutDialog;
    double zoomInNum = 0.5;
    double zoomOutNum = 0.5;
    int qrcodeWidth = 200;
    int qrcodeHeight = 200;
    void readImage();
    void readText();
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    Ui::MainWindow *ui;
    QGraphicsScene *myScene;
    QList<QGraphicsItem*> list;
public slots:
    void loadFile(QGraphicsSceneMouseEvent *mouseEvent);
    void setLoadType();
    void setZoomInNum(double a);
    void setZoomOutNum(double a);
    void setQRcodeWidth(int w);
    void setQRcodeHeight(int h);
private slots:
    void showLoc();
    void on_deleteBtn_clicked();
    void on_zoomInBtn_clicked();
    void on_zoomOutBtn_clicked();
    void showFontComboBox(QString);
    void showSizeComboBox(QString);
    void on_boldBtn_clicked(bool checked);
    void on_italicBtn_clicked(bool checked);
    void on_underlineBtn_clicked(bool checked);
    void on_addxBtn_clicked();
    void on_reducexBtn_clicked();
    void on_addyBtn_clicked();
    void on_reduceyBtn_clicked();
    void on_saveBtn_clicked();
    void on_readBtn_clicked();
    void on_saveImageBtn_clicked();
    void on_modifyBtn_clicked();
    void showSetDialog();
    void showAboutDialog();
};

#endif // MAINWINDOW_H
