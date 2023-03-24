#include "geometrictrans.h"
#include "ui_geometrictrans.h"

GeometricTrans::GeometricTrans(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeometricTrans)
{
    ui->setupUi(this);
    setWindowTitle("图像几何变换");
    ui->lineEditR->setText("0");
    ui->lineEditRx->setText("0");
    ui->lineEditRy->setText("0");
    ui->lineEditSX->setText("1.0");
    ui->lineEditSY->setText("1.0");


}

GeometricTrans::~GeometricTrans()
{
    delete ui;
}

void GeometricTrans::setSize(int width, int height)
{
    w=width;
    h=height;
    ui->lineEditW->setText(QString::number(w));
    ui->lineEditH->setText(QString::number(h));
    ui->lineEditW->setFocusPolicy(Qt::NoFocus);
    ui->lineEditH->setFocusPolicy(Qt::NoFocus);
}

void GeometricTrans::on_pushButtonConfirm_clicked()
{
    if(ui->lineEditRx->text().toInt()>=0&&ui->lineEditRx->text().toInt()<w&&ui->lineEditRy->text().toInt()>=0&&ui->lineEditRy->text().toInt()<h&&ui->lineEditSX->text().toDouble()>0&&ui->lineEditSY->text().toDouble()>0)
    {
     if(mainWindow->count>=1){
     mainWindow->ex_rh=h;
     mainWindow->ex_rw=w;
     }
     mainWindow->rotationAngle=ui->lineEditR->text().toDouble();
     mainWindow->rx=ui->lineEditRx->text().toInt();
     mainWindow->ry=ui->lineEditRy->text().toInt();
     mainWindow->sx=ui->lineEditSX->text().toDouble();
     mainWindow->sy=ui->lineEditSY->text().toDouble();
     mainWindow->startTrans=true;
     this->close();
    }
    else
    {
        QMessageBox::information(this,"提示","参数格式错误");
    }



}

void GeometricTrans::on_pushButtonBack_clicked()
{
    this->close();
}
