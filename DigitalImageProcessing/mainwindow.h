#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include"QMessageBox"
#include"QCloseEvent"
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include  "imageprocess.h"
#include <QDebug>
#include <stack>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void LoadImage(QString filename);//加载图像
    ~MainWindow();
    QImage image,imageOut;
    ImageProcess imgP;
    bool hasImage,startTrans,startChangeWin,hasProcess,startEnhance,control;
    int rx,ry,pos,wid,curval;
    double sx,sy,s;
    double rotationAngle;
    int ex_rh,ex_rw;
    int count;
     std::stack<int>ex_pos,ex_wid;
protected: //keyboard
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private slots:
    void on_actionload_triggered();

    void on_actionexit_triggered();

    void on_actionWindow_triggered();

    void on_actionsaveImage_triggered();

    void on_actionenhance_triggered();

    void on_actionreverse_triggered();

    void on_actionturnAround_triggered();

    void on_actionrestore_triggered();

    void on_verticalSlider_valueChanged(int value);

    void on_actiongeoTrans_triggered();

    void on_actionstepback_triggered();

    void on_verticalSlider_sliderReleased();

private:
    Ui::MainWindow *ui;
    std::stack<int>op;
    std::stack<double>ex_s;

};
#endif // MAINWINDOW_H  
