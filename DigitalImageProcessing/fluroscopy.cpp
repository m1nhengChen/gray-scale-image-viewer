#include "fluroscopy.h"
#include "ui_fluroscopy.h"

Fluroscopy::Fluroscopy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Fluroscopy)
{
    ui->setupUi(this);
    setWindowTitle("窗位窗宽调整");

}

Fluroscopy::~Fluroscopy()
{
    delete ui;
}

void Fluroscopy::setParameter(int p, int w)
{
    pos=p;
    wid=w;
    ui->lineEditPos->setText(QString::number(pos));
    ui->lineEditW->setText(QString::number(wid));

}


void Fluroscopy::on_pushButtonConfirm_clicked()
{

    int mid=ui->lineEditPos->text().toInt();
    int t_wid=ui->lineEditW->text().toInt();
    int low=mid-t_wid/2;
    if(low>=0&&t_wid>1)
    {
        if(mainWindow->count>=1)
        {
        mainWindow->ex_pos.push(pos);
        mainWindow->ex_wid.push(wid);
        }

        mainWindow->count++;
        mainWindow->pos=mid;
        mainWindow->wid=t_wid;
        mainWindow->startChangeWin=true;
        //qDebug()<<mainWindow->startChangeWin;
        this->close();
    }
    else
    {
       QMessageBox::information(this,"提示","参数格式错误");
    }


}

void Fluroscopy::on_pushButtonBack_clicked()
{
     this->close();
}
