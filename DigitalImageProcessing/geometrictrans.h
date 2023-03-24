#ifndef GEOMETRICTRANS_H
#define GEOMETRICTRANS_H
#include<QDebug>
#include <QDialog>
#include"mainwindow.h"
namespace Ui {
class GeometricTrans;
}

class GeometricTrans : public QDialog
{
    Q_OBJECT

public:
    explicit GeometricTrans(QWidget *parent = nullptr);
    ~GeometricTrans();
    MainWindow *mainWindow;
    int w,h;
void setSize(int width,int height);
private slots:
    void on_pushButtonConfirm_clicked();
    void on_pushButtonBack_clicked();

private:
    Ui::GeometricTrans *ui;
};

#endif // GEOMETRICTRANS_H
