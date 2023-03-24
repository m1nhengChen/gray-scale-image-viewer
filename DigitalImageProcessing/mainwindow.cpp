#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"geometrictrans.h"
#include"fluroscopy.h"
#include"iostream"
#include"QDataStream"
#include"QFile"
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("数字图像处理");
    hasImage=false;
    startTrans=false;
    startChangeWin=false;
    ui->verticalSlider->setMaximum(100);
    ui->verticalSlider->setMinimum(0);
    ui->verticalSlider->setSingleStep(5);

    ui->verticalSlider->setTickInterval(5);
    ui->verticalSlider->setTickPosition(QSlider::TicksRight);
    curval=50;
    ui->verticalSlider->setValue(curval);
    ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
    control=false;

}

void MainWindow::LoadImage(QString filename)
{
    if(filename.endsWith(".bmp")||filename.endsWith(".BMP"))
    {
    //bmp格式加载
    QImage img;
    if(!(img.load(filename))) //加载图像
    {
        QMessageBox::information(this, tr("打开图像失败"),tr("打开图像失败!"));
        return;
    }
    ui->inputLabel->setPixmap(QPixmap::fromImage(img));
    ui->inputLabel->adjustSize();
    ui->inputScrollArea->setWidget(ui->inputLabel);
    this->image=img;
    imgP.setBmpData(this->image);//初始化图像处理类中的数据
    ex_rh=img.height();
    ex_rw=img.width();
    pos=128;
    wid=256;
    curval=50;
    ui->verticalSlider->setValue(curval);
    ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
    count=0;
    s=1;
    if(hasImage)ui->outputLabel->clear();
    while(!op.empty())op.pop();
    while(!ex_s.empty())ex_s.pop();
    }
    else if(filename.endsWith(".jpg"))
    {

    }
    else if(filename.endsWith(".raw"))
    {
        unsigned char c;
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        QDataStream stream(&file);
        unsigned long width=0;unsigned long height=0;
        //读取宽度
        for(int i=0;i<4;i++)
             {
                    stream>>c;width=width+(c<<(8*i));
             }
        //读取高度
        for(int i=0;i<4;i++)
            {
                    stream>>c;height=height+(c<<(8*i));
             }
        //读取图片像素值
        unsigned short* pixel=new unsigned short[width*height];
        for(int i=0;i<height;i++)
                {
                    for(int j=0;j<width;j++)
                    {
                        stream>>c;
                        pixel[i*width+j]=c;
                        stream>>c;
                        pixel[i*width+j]=pixel[i*width+j]+(c<<8);
                    }
                }

          QImage input=imgP.setRawData(pixel,width,height);
          delete []pixel;
          ui->inputLabel->setPixmap(QPixmap::fromImage(input));
          ui->inputLabel->adjustSize();
          ui->inputScrollArea->setWidget(ui->inputLabel);
          this->image=input;
          file.close();
          pos=128;
          wid=256;
          curval=50;
          s=1;
          ui->verticalSlider->setValue(curval);
          ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
          if(hasImage)ui->outputLabel->clear();
          count=0;
          while(!op.empty())op.pop();
          while(!ex_s.empty())ex_s.pop();
          while (!ex_wid.empty()) ex_wid.pop();
          while(!ex_pos.empty())ex_pos.pop();


    }
    else
    {
        QMessageBox::warning(this,"警告","文件格式无法识别");
        return;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionload_triggered()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("选择图像"),"",tr("Images (*.png *.bmp *.jpg *.raw)"));
        if(filename.isEmpty())
        {
            QMessageBox::information(this,"提示","文件路径不能为空");
            return;
        }
        else
        {

            LoadImage(filename);
            hasImage=true;
            hasProcess=false;

        }

}

void MainWindow::on_actionexit_triggered()
{
    if(QMessageBox::Yes==  QMessageBox::question(this,"提示","确认退出？"))
        this->close();
}

void MainWindow::on_actionWindow_triggered()
{
    if(hasImage)
    {
        Fluroscopy dlg;
        dlg.setParameter(pos,wid);
        dlg.mainWindow=this;
        dlg.exec();
        if(startChangeWin)
        {
            QImage output=imgP.flouroscopyImage(pos,wid);

            //qDebug()<<output.height()<<output.width();
            ui->inputLabel->setPixmap(QPixmap::fromImage(output));
            ui->inputLabel->adjustSize();
            ui->inputScrollArea->setWidget(ui->inputLabel);
            this->image=output;
            output=imgP.grayScaleTransform(pos,wid,hasProcess);
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            startChangeWin=false;
            hasProcess=true;
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            op.push(0);
            qDebug()<<count;

        }

    }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }

}

void MainWindow::on_actionsaveImage_triggered()
{if(hasImage)
    {
        if(hasProcess)
        {
            QString filename=QFileDialog::getSaveFileName(this,tr("保存图像"),"","BMP(*.bmp);;JPG(*.jpg);;PNG(*.png)");
                if(filename.isEmpty())
                {
                    QMessageBox::information(this,"提示","保存路径不能为空");
                    return;
                }
                else
                {
                    if(filename.endsWith(".bmp"))
                    {
                    imageOut.save(filename,"bmp",100);
                    }

                    else if(filename.endsWith(".jpg"))
                    {
                     imageOut.save(filename,"jpg",100);
                    }
                    else if(filename.endsWith(".png"))
                    {
                        imageOut.save(filename,"png",100);
                    }

                }
        }
        else
        {
            QMessageBox::information(this,"提示","请先处理图片");
            return;
        }

    }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }

}

void MainWindow::on_actionenhance_triggered()
{

    if(hasImage)
    {
            qDebug()<<hasProcess;
            if(count>=1)imgP.save();

            imgP.imageEnhanced(hasProcess);
            hasProcess=true;
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
            //qDebug()<<output.height()<<output.width();
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            this->imageOut=output; 
            count++;
            op.push(1);

    }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }
}

void MainWindow::on_actionreverse_triggered()
{
    if(hasImage)
        {

                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                imgP.reverse(hasProcess);
                hasProcess=true;
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                count++;
                op.push(2);
        }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }

}

void MainWindow::on_actionturnAround_triggered()
{
    if(hasImage)
        {

                imgP.turnAround(hasProcess);
                 hasProcess=true;
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                count++;
                op.push(3);
        }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }
}

void MainWindow::on_actionrestore_triggered()
{
    if(hasImage)
        {
            if(hasProcess)
            {
                QImage output=imgP.restore();
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                hasProcess=true;
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                count=1;
                while (!op.empty()) op.pop();
                while(!ex_s.empty())ex_s.pop();
                op.push(4);
                imgP.rh=imgP.h;
                imgP.rw=imgP.w;
                pos=128;
                wid=256;
            }
            else
            {
                QMessageBox::information(this,"提示","请先处理图片");
                return;
            }

        }
        else
        {
            QMessageBox::information(this,"提示","请先载入图片");
            return;
        }
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    ui->label_3->setText("缩放系数:"+QString::number(double(value)/50));
    return;

}
void MainWindow::on_actiongeoTrans_triggered()
{if(hasImage)
    {
        GeometricTrans dlg;
        dlg.mainWindow=this;
        dlg.setSize(image.width(),image.height());
        dlg.exec();
        if(startTrans)
        {
        imgP.save();
        imgP.geometryTransform(sx,sy,rx,ry,rotationAngle);
        hasProcess=true;
        QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
        imageOut=output;
        ui->outputLabel->setPixmap(QPixmap::fromImage(output));
        ui->outputLabel->adjustSize();
        ui->outputScrollArea->setWidget(ui->outputLabel);
        startTrans=false;
        ui->verticalSlider->setValue(50);
        ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
        count++;
        op.push(6);
        }

    }
    else
    {
        QMessageBox::information(this,"提示","请先载入图片");
        return;
    }

}

void MainWindow::on_actionstepback_triggered()
{
    qDebug()<<count;
    if(count>=2)
    {
        int status=op.top();
        switch(status){
        case 0:
        {
               QImage output=imgP.flouroscopyImage(ex_pos.top(),ex_wid.top());
                ui->inputLabel->setPixmap(QPixmap::fromImage(output));
                ui->inputLabel->adjustSize();
                ui->inputScrollArea->setWidget(ui->inputLabel);
                this->image=output;
                output=imgP.grayScaleTransform(ex_pos.top(),ex_wid.top(),hasProcess);
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                startChangeWin=false;
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                pos=ex_pos.top();
                wid=ex_wid.top();
                count--;
                ex_pos.pop();
                ex_wid.pop();
                break;
        }

        case 1:
        {
            imgP.copy(ex_rh,ex_rw);
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
            //qDebug()<<output.height()<<output.width();
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            this->imageOut=output;
            hasProcess=true;
            while(!op.empty())op.pop();
            while(!ex_s.empty())ex_s.pop();
            count=1;
            op.push(1);
            break;
        }
        case 2:
        {
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            imgP.reverse(hasProcess);
            hasProcess=true;
            QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            this->imageOut=output;

            count--;
            op.pop();
            break;
        }
        case 3:
        {
            imgP.turnAround(hasProcess);
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            this->imageOut=output;
            hasProcess=true;
            count--;
            op.pop();
            break;
        }
        case 4:
            QMessageBox::information(this,"提示","请先进行操作");
            return;
            break;
        case 5:
        {

            s=ex_s.top();
            ex_s.pop();
            QImage output=imgP.geometryTransform_r(s,s,0,0,0,pos,wid);
            imageOut=output;
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            startTrans=false;
            hasProcess=true;
            ui->label_3->setText("缩放系数:"+QString::number(s));
            count--;
            op.pop();
            break;
        }
        case 6:
        {
            imgP.copy(ex_rh,ex_rw);
            QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
            imageOut=output;
            ui->outputLabel->setPixmap(QPixmap::fromImage(output));
            ui->outputLabel->adjustSize();
            ui->outputScrollArea->setWidget(ui->outputLabel);
            startTrans=false;
            hasProcess=true;
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
            while(!op.empty())op.pop();
            while(!ex_s.empty())ex_s.pop();
            count=1;
            op.push(6);
            break;
        }
        default:
            break;
        }
    }
    else if(count==1)
    {
        hasProcess=false;
        hasImage=false;
        imgP.clear();
        ui->outputLabel->clear();
        ui->verticalSlider->setValue(50);
        ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
    }
    else
    {
        QMessageBox::information(this,"提示","亲^3^，不能再继续撤销了哦~");
        return;
    }
}

void MainWindow::on_verticalSlider_sliderReleased()
{
    int value=ui->verticalSlider->value();
    if(hasImage)
    {
        if(hasProcess)
        {
        qDebug()<<s;
        if(count>=1)
        ex_s.push(s);
        s=double(value)/50;
        QImage output=imgP.geometryTransform_r(s,s,0,0,0,pos,wid);
        imageOut=output;
        ui->outputLabel->setPixmap(QPixmap::fromImage(output));
        ui->outputLabel->adjustSize();
        ui->outputScrollArea->setWidget(ui->outputLabel);
        startTrans=false;
        hasProcess=true;
        ui->label_3->setText("缩放系数:"+QString::number(s));
        count++;
        op.push(5);
        }
        else
        {
            ui->label_3->setText("缩放系数:"+QString::number(double(value)/50));
            return;
        }


    }
    else
    {
        ui->label_3->setText("缩放系数:"+QString::number(double(value)/50));
        return;
    }
}
// 键盘按下事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{

    int keyValue = event->key();
    if(keyValue==Qt::Key_Control)
        control=true;
    qDebug() << "=== press key"<<control;
}



// 按键释放事件
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "    release key ==="<<control;
    int keyValue = event->key();

    if (keyValue == Qt::Key_F)
    {
       if(control)
       {
        if(count>=2)
        {
            int status=op.top();
            qDebug() << status;
            switch(status){
            case 0:
            {
                QImage output=imgP.flouroscopyImage(ex_pos.top(),ex_wid.top());
                 ui->inputLabel->setPixmap(QPixmap::fromImage(output));
                 ui->inputLabel->adjustSize();
                 ui->inputScrollArea->setWidget(ui->inputLabel);
                 this->image=output;
                 output=imgP.grayScaleTransform(ex_pos.top(),ex_wid.top(),hasProcess);
                 ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                 ui->outputLabel->adjustSize();
                 ui->outputScrollArea->setWidget(ui->outputLabel);
                 startChangeWin=false;
                 ui->verticalSlider->setValue(50);
                 ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                 pos=ex_pos.top();
                 wid=ex_wid.top();
                 count--;
                 ex_pos.pop();
                 ex_wid.pop();
                 break;
            }

            case 1:
            {
                imgP.copy(ex_rh,ex_rw);
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                //qDebug()<<output.height()<<output.width();
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                hasProcess=true;
                while(!op.empty())op.pop();
                while(!ex_s.empty())ex_s.pop();
                count=1;
                op.push(1);
                break;
            }
            case 2:
            {
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                imgP.reverse(hasProcess);
                hasProcess=true;
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                count--;
                op.pop();
                break;
            }
            case 3:
            {
                imgP.turnAround(hasProcess);
                hasProcess=true;
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                this->imageOut=output;
                count--;
                op.pop();
                break;
            }
            case 4:
                QMessageBox::information(this,"提示","请先进行操作");
                return;
                break;
            case 5:
            {

                s=ex_s.top();
                ex_s.pop();
                QImage output=imgP.geometryTransform_r(s,s,0,0,0,pos,wid);
                imageOut=output;
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                startTrans=false;
                hasProcess=true;
                ui->label_3->setText("缩放系数:"+QString::number(s));
                count--;
                op.pop();
                break;
            }
            case 6:
            {
                imgP.copy(ex_rh,ex_rw);
                QImage output=imgP.grayScaleTransform(pos,wid,hasProcess);
                imageOut=output;
                ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                ui->outputLabel->adjustSize();
                ui->outputScrollArea->setWidget(ui->outputLabel);
                startTrans=false;
                hasProcess=true;
                hasProcess=true;
                ui->verticalSlider->setValue(50);
                ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                while(!op.empty())op.pop();
                while(!ex_s.empty())ex_s.pop();
                count=1;
                op.push(6);
                break;
            }
            default:
                break;
            }
        }
        else if(count==1)
        {
            hasProcess=false;
            hasImage=false;
            imgP.clear();
            ui->outputLabel->clear();
            ui->verticalSlider->setValue(50);
            ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
        }
        else

        {
            QMessageBox::information(this,"提示","亲^3^，不能再继续撤销了哦~");
            return;
        }
    }
    }
    else if(keyValue==Qt::Key_Control)
    {
        control=false;
    }
    else if(keyValue==Qt::Key_W)
    {
        if(control)
        {
            if(hasImage)
            {
                Fluroscopy dlg;
                dlg.setParameter(pos,wid);
                dlg.mainWindow=this;
                dlg.exec();
                if(startChangeWin)
                {
                    QImage output=imgP.flouroscopyImage(pos,wid);

                    //qDebug()<<output.height()<<output.width();
                    ui->inputLabel->setPixmap(QPixmap::fromImage(output));
                    ui->inputLabel->adjustSize();
                    ui->inputScrollArea->setWidget(ui->inputLabel);
                    this->image=output;
                    if(hasProcess)
                    output=imgP.grayScaleTransform(pos,wid,hasProcess);
                    ui->outputLabel->setPixmap(QPixmap::fromImage(output));
                    ui->outputLabel->adjustSize();
                    ui->outputScrollArea->setWidget(ui->outputLabel);
                    startChangeWin=false;
                    ui->verticalSlider->setValue(50);
                    ui->label_3->setText("缩放系数:"+QString::number(double(curval)/50));
                    op.push(0);
                    //qDebug()<<count;

                }

            }
            else
            {
                QMessageBox::information(this,"提示","请先载入图片");
                return;
            }
        }
    }
    else
        return;
}
