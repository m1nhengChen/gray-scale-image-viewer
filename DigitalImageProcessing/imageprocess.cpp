#include "imageprocess.h"

#include <QDebug>
ImageProcess::ImageProcess()
{
    init_pixel_table();
    inputData=nullptr;
    outputData=nullptr;
    savemodel=nullptr;
    sobelImage=nullptr;

}

void ImageProcess::setBmpData(QImage image)
{
   w=image.width();
   h=image.height();
   rw=w;
   rh=h;
   inputData=new int[w*h];
   highest_intensity=255;
  data = new RgbVector2D;
      // 动态设置大小.
      data->resize(w);
      for(int i=0; i<w; i++)
      {
          (*data)[i].resize(h);
      }

      for(int i=0; i<w; i++)
      {
          for(int j=0; j<h; j++)
          {
              (*data)[i][j] = image.pixelIndex(i,j);
              inputData[i*h+j]=(*data)[i][j];
          }
      }
}

QImage ImageProcess::setRawData(unsigned short *input, int width, int height)
{
    w=width;
    h=height;
    rw=w;
    rh=h;
    inputData=new int[w*h];
    highest_intensity=4095;
    qDebug()<<"a";
    QImage output(width,height,QImage::Format_Indexed8);
    output.setColorTable(pixelTable);
    data = new RgbVector2D;
        // 动态设置大小.
    data->resize(w);
    for(int i=0; i<w; i++)
        {
            (*data)[i].resize(h);
        }

   for(int i=0;i<height;i++)
       for(int j=0;j<width;j++)
       {
           (*data)[j][i]=input[j+i*width];
           inputData[j*h+i]=(*data)[j][i];
           output.setPixel(j,i,getPixel((*data)[j][i]));
       }
   return output;
}

void ImageProcess::geometryTransform(double sx,double sy,int rx,int ry,double r)
{
    double x_data,y_data;
    if(outputData!=nullptr)delete [] outputData;
    //求出缩放后的图像的尺寸大小
    int resultWidth=ceil(w*sx);
    int resultHeight=ceil(h*sy);
    rw=resultWidth;
    rh=resultHeight;
    //qDebug()<<r;
    r=qDegreesToRadians(r);
    //qDebug()<<r;
    outputData=new int[resultWidth*resultHeight];
    for(int i=0; i<resultWidth; i++)
    {
        for(int j=0; j<resultHeight; j++)
        {
            //反向计算结果图像各个点的像素值
            // i=sx*[cosr(x-rx)-sinr(y-ry)+rx]
            // j=sy*[sinr(x-rx)+cosr(y-ry)+ry]

            x_data=qCos(-r)*(double(i)/sx-rx)-qSin(-r)*(double(j)/sy-ry)+rx;
            y_data=qSin(-r)*(double(i)/sx-rx)+qCos(-r)*(double(j)/sy-ry)+ry;
            if(x_data>w-1||x_data<0||y_data>h-1||y_data<0)
            {
                outputData[i*resultHeight+j]=0;
            }
            else
            {
                //双线性插值
                int x_min=floor(x_data);
                int x_max=ceil(x_data);
                int y_min=floor(y_data);
                int y_max=ceil(y_data);
                int intensity_0=(*data)[x_min][y_min];
                int intensity_1=(*data)[x_max][y_min];
                int intensity_4=(*data)[x_min][y_max];
                int intensity_5=(*data)[x_max][y_max];
                int intensity_2,intensity_3,intensity;
                if(intensity_4==intensity_0)
                {
                    intensity_2=intensity_0;
                }
                else
                {
                    intensity_2=intensity_4*(y_data-y_min)+intensity_0*(y_max-y_data);
                }
                if(intensity_5==intensity_1)
                {
                    intensity_3=intensity_1;
                }
                else
                {
                    intensity_3=intensity_5*(y_data-y_min)+intensity_1*(y_max-y_data);
                }
                if(intensity_2==intensity_3)
                {
                    intensity=intensity_2;
                }
                else
                {
                    intensity=intensity_3*(x_data-x_min)+intensity_2*(x_max-x_data);
                }
               outputData[i*resultHeight+j]=intensity;
                //qDebug()<<intensity;
            }
        }
    }
}

QImage ImageProcess::geometryTransform_r(double sx, double sy, int rx, int ry, double r,int pos,int wid)
{
    double x_data,y_data;
    //求出缩放后的图像的尺寸大小
    int low=pos-wid/2;
    int resultWidth=ceil(rw*sx);
    int resultHeight=ceil(rh*sy);
    qDebug()<<r;
    r=qDegreesToRadians(r);
    QImage result(resultWidth,resultHeight,QImage::Format_Indexed8);
    result.setColorTable(pixelTable);
    for(int i=0; i<resultWidth; i++)
    {
        for(int j=0; j<resultHeight; j++)
        {
            //反向计算结果图像各个点的像素值
            // i=sx*[cosr(x-rx)-sinr(y-ry)+rx]
            // j=sy*[sinr(x-rx)+cosr(y-ry)+ry]

            x_data=qCos(-r)*(double(i)/sx-rx)-qSin(-r)*(double(j)/sy-ry)+rx;
            y_data=qSin(-r)*(double(i)/sx-rx)+qCos(-r)*(double(j)/sy-ry)+ry;
            if(x_data>w-1||x_data<0||y_data>h-1||y_data<0)
            {
                result.setPixel(i,j,getPixel(0));
            }
            else
            {
                //双线性插值
                int x_min=floor(x_data);
                int x_max=ceil(x_data);
                int y_min=floor(y_data);
                int y_max=ceil(y_data);
                int intensity_0=outputData[x_min*rh+y_min];
                int intensity_1=outputData[x_max*rh+y_min];
                int intensity_4=outputData[x_min*rh+y_max];
                int intensity_5=outputData[x_max*rh+y_max];
                int intensity_2,intensity_3,intensity;
                if(intensity_4==intensity_0)
                {
                    intensity_2=intensity_0;
                }
                else
                {
                    intensity_2=intensity_4*(y_data-y_min)+intensity_0*(y_max-y_data);
                }
                if(intensity_5==intensity_1)
                {
                    intensity_3=intensity_1;
                }
                else
                {
                    intensity_3=intensity_5*(y_data-y_min)+intensity_1*(y_max-y_data);
                }
                if(intensity_2==intensity_3)
                {
                    intensity=intensity_2;
                }
                else
                {
                    intensity=intensity_3*(x_data-x_min)+intensity_2*(x_max-x_data);
                }
               result.setPixel(i,j,getPixel((intensity-low)*256/wid));
            }
        }
    }

    return result;
}

void ImageProcess::init_pixel_table()
{
    for(int i=0;i<256;i++)
    {
        pixelTable.push_back(qRgb(i,i,i));
    }
}

void ImageProcess::reverse(bool hasprocess)
{
    if(hasprocess)
    {
    for(int i=0;i<rw;i++)
        for (int j=0;j<rh;j++)
        {
            outputData[i*rh+j]=abs(highest_intensity- outputData[i*rh+j]);
        }
    }
    else
    {
        outputData=new int[rh*rw];
        for(int i=0;i<w;i++)
            for (int j=0;j<h;j++)
            {
                outputData[i*h+j]=abs(highest_intensity-(*data)[i][j]);
            }
    }
}

void ImageProcess::turnAround(bool hasprocess)
{
    int temp;
    if(hasprocess)
    {
    for(int i=0;i<rw/2;i++)
        for (int j=0;j<rh;j++)
        {
            temp=outputData[i*rh+j];
            outputData[i*rh+j]=outputData[(rw-i-1)*rh+j];
            outputData[(rw-i-1)*rh+j]=temp;
        }
    }
    else
    {
        if(outputData!=nullptr)delete[]outputData;
        outputData=new int[rh*rw];
        for(int i=0;i<rw;i++)
            for (int j=0;j<rh;j++)
            {
            outputData[i*rh+j]=(*data)[i][j];
            }
        for(int i=0;i<rw/2;i++)
            for (int j=0;j<rh;j++)
            {
                temp=outputData[i*rh+j];
                outputData[i*rh+j]=outputData[(rw-i-1)*rh+j];
                outputData[(rw-i-1)*rh+j]=temp;
            }
    }
}

QImage ImageProcess::restore()
{
    delete []outputData;
    outputData=new int[w*h];
    rw=w;
    rh=h;
    QImage result(rw,rh,QImage::Format_Indexed8);
    result.setColorTable(pixelTable);
    for(int i=0;i<rw;i++)
        for (int j=0;j<rh;j++)
        {
        outputData[i*rh+j]=inputData[i*rh+j];
        result.setPixel(i,j,getPixel(outputData[i*h+j]));
        }
    return result;
}

void ImageProcess::clear()
{
    delete[]outputData;
    outputData=nullptr;
}

void ImageProcess::copy(int ex_rh,int ex_rw)
{
    //qDebug()<<ex_rh<<ex_rw;
    delete[] outputData;
    outputData=new int[ex_rh*ex_rw];
    rh=ex_rh;
    rw=ex_rw;
    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
            outputData[i*rh+j]=savemodel[i*rh+j];
    delete[] savemodel;
}

QImage ImageProcess::grayScaleTransform(int pos, int wid,bool hasprocess)
{

    QImage result(rw,rh,QImage::Format_Indexed8);
    result.setColorTable(pixelTable);
    //计算像素的最大与最小值
    if(!hasprocess)
    {
        outputData=new int[rw*rh];
        for(int i=0;i<rw;i++)
            for(int j=0;j<rh;j++)
            outputData[i*rh+j]=(*data)[i][j];
    }
    int low=pos-wid/2;
    for(int i=0;i<rw;i++)
        for (int j=0;j<rh;j++)
        {
            result.setPixel(i,j,getPixel((outputData[i*rh+j]-low)*256/wid));
        }
    //将结果转换为QImage格式，并返回至mainwindow
    return result;

}



void ImageProcess::imageEnhanced(bool hasprocess)
{
    //计算像素的最大与最小值
    //拉普拉斯算子卷积核
    double c=-0.0025;//边界锐化系数

//    int laplace4[3][3]={0,1,0,1,-4,1,0,1,0};
//    int laplace8[3][3]={1,1,1,1,-8,1,1,1,1};
// 获取sobel图像
if(hasprocess)
{
    sobelImage= new int[rw*rh];
    int*copy=new int[rw*rh];
    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
        {
            copy[i*rh+j]=outputData[i*rh+j];
        }


    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
        {
            sobelImage[i*rh+j]=sobel_r(i,j);
        }
    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
        {

                //result[i*h+j]=(laplace4Cov(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+c*laplace4Cov(i,j)-low)*256/wid;
                //result[i*h+j]=(laplace8Cov(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+c*laplace8Cov(i,j)-low)*256/wid;
                //result[i*h+j]=(sobel(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+sobel(i,j)-low)*256/wid;
                //result[i*h+j]=(gaussfiltering(i,j)-low)*256/wid;
                copy[i*rh+j]=outputData[i*rh+j]+c*gaussfiltering(i,j)*laplace8Cov_r(i,j);
        }
    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
        {
            outputData[i*rh+j]=copy[i*rh+j];
        }
    delete [] copy;
    delete[]sobelImage;
}
else
{
    sobelImage= new int[w*h];
    outputData=new int[rw*rh];
    for(int i=0;i<w;i++)
        for(int j=0;j<h;j++)
        {
            sobelImage[i*h+j]=sobel(i,j);
        }
    for(int i=0;i<w;i++)
        for(int j=0;j<h;j++)
        {

                //result[i*h+j]=(laplace4Cov(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+c*laplace4Cov(i,j)-low)*256/wid;
                //result[i*h+j]=(laplace8Cov(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+c*laplace8Cov(i,j)-low)*256/wid;
                //result[i*h+j]=(sobel(i,j)-low)*256/wid;
                //result[i*h+j]=((*data)[i][j]+sobel(i,j)-low)*256/wid;
                //result[i*h+j]=(gaussfiltering(i,j)-low)*256/wid;
                outputData[i*h+j]=(*data)[i][j]+c*gaussfiltering(i,j)*laplace8Cov(i,j);

        }
   delete[]sobelImage;
}
}


QImage ImageProcess::flouroscopyImage(int pos,int wid)
{

    //计算像素的最大与最小值
    inputData=new int[h*w];
    int low=pos-wid/2;int high=pos+wid/2;
    for(int i=0;i<w;i++)
        for (int j=0;j<h;j++)
        {
            if(low<=(*data)[i][j]&&(*data)[i][j]<high)
            {
                inputData[i*h+j]=((*data)[i][j]-low)*256/wid;
            }
            else if((*data)[i][j]<low)
            {
                inputData[i*h+j]=0;
            }
            else if((*data)[i][j]>=high)
            {
                inputData[i*h+j]=255;
            }
        }
    //将结果转换为QImage格式，并返回至mainwindow
    QImage result(w,h,QImage::Format_Indexed8);
    result.setColorTable(pixelTable);
    for(int i=0; i<w; i++)
    {
        for(int j=0; j<h; j++)
        {
             result.setPixel(i,j,getPixel(inputData[i*h+j]));
        }
    }
    return result;

}

int ImageProcess::laplace4Cov(int x, int y)
{
    int cov10=0,cov01=0,cov11=(*data)[x][y],cov21=0,cov12=0;
    if(y-1<=h-1&&y-1>=0)
        cov10=(*data)[x][y-1];
    if(x-1<=w-1&&x-1>=0)
        cov01=(*data)[x-1][y];
    if(x+1<=w-1&&x+1>=0)
        cov21=(*data)[x+1][y];
    if(y+1<=h-1&&y+1>=0)
        cov12=(*data)[x][y+1];
    return(cov01+cov10+cov12+cov21-4*cov11);


}

int ImageProcess::laplace8Cov(int x, int y)
{
    int cov00=0,cov10=0,cov20=0,cov01=0,cov11=(*data)[x][y],cov21=0,cov02=0,cov12=0,cov22=0;
    if(y-1<=h-1&&y-1>=0&&x-1<=w-1&&x-1>=0)
        cov00=(*data)[x-1][y-1];
    if(y-1<=h-1&&y-1>=0)
        cov10=(*data)[x][y-1];
    if(y-1<=h-1&&y-1>=0&&x+1<=w-1&&x+1>=0)
        cov20=(*data)[x+1][y-1];
    if(x-1<=w-1&&x-1>=0)
        cov01=(*data)[x-1][y];
    if(x+1<=w-1&&x+1>=0)
        cov21=(*data)[x+1][y];
    if(y+1<=h-1&&y+1>=0&&x-1<=w-1&&x-1>=0)
        cov02=(*data)[x-1][y+1];
    if(y+1<=h-1&&y+1>=0)
        cov12=(*data)[x][y+1];
    if(y+1<=h-1&&y+1>=0&&x+1<=w-1&&x+1>=0)
        cov22=(*data)[x+1][y+1];
    return(cov00+cov01+cov02+cov10+cov12+cov20+cov21+cov22-8*cov11);
}

int ImageProcess::laplace8Cov_r(int x, int y)
{
    int cov00=0,cov10=0,cov20=0,cov01=0,cov11=outputData[x*rh+y],cov21=0,cov02=0,cov12=0,cov22=0;
    if(y-1<=rh-1&&y-1>=0&&x-1<=rw-1&&x-1>=0)
        cov00=outputData[(x-1)*rh+y-1];
    if(y-1<=rh-1&&y-1>=0)
        cov10=outputData[x*rh+y-1];
    if(y-1<=rh-1&&y-1>=0&&x+1<=rw-1&&x+1>=0)
        cov20=outputData[(x+1)*rh+y-1];
    if(x-1<=rw-1&&x-1>=0)
        cov01=outputData[(x-1)*rh+y];
    if(x+1<=rw-1&&x+1>=0)
        cov21=outputData[(x+1)*rh+y];
    if(y+1<=rh-1&&y+1>=0&&x-1<=rw-1&&x-1>=0)
        cov02=outputData[(x-1)*rh+y+1];
    if(y+1<=rh-1&&y+1>=0)
        cov12=outputData[x*rh+y+1];
    if(y+1<=rh-1&&y+1>=0&&x+1<=rw-1&&x+1>=0)
        cov22=outputData[(x+1)*rh+y+1];
    return(cov00+cov01+cov02+cov10+cov12+cov20+cov21+cov22-8*cov11);
}

int ImageProcess::sobel(int x, int y)
{
    int cov00=0,cov10=0,cov20=0,cov01=0,cov21=0,cov02=0,cov12=0,cov22=0;
    if(y-1<=h-1&&y-1>=0&&x-1<=w-1&&x-1>=0)
        cov00=(*data)[x-1][y-1];
    if(y-1<=h-1&&y-1>=0)
        cov10=(*data)[x][y-1];
    if(y-1<=h-1&&y-1>=0&&x+1<=w-1&&x+1>=0)
        cov20=(*data)[x+1][y-1];
    if(x-1<=w-1&&x-1>=0)
        cov01=(*data)[x-1][y];
    if(x+1<=w-1&&x+1>=0)
        cov21=(*data)[x+1][y];
    if(y+1<=h-1&&y+1>=0&&x-1<=w-1&&x-1>=0)
        cov02=(*data)[x-1][y+1];
    if(y+1<=h-1&&y+1>=0)
        cov12=(*data)[x][y+1];
    if(y+1<=h-1&&y+1>=0&&x+1<=w-1&&x+1>=0)
        cov22=(*data)[x+1][y+1];
    int horizontal_sobel=-cov00-2*cov10-cov20+cov02+2*cov12+cov22;
    int vertical_sobel=-cov00+cov20-2*cov01+2*cov21-cov02+cov22;
    return floor(qSqrt(qPow(horizontal_sobel,2)+qPow(vertical_sobel,2)));
}

int ImageProcess::sobel_r(int x, int y)
{
    int cov00=0,cov10=0,cov20=0,cov01=0,cov21=0,cov02=0,cov12=0,cov22=0;
    if(y-1<=h-1&&y-1>=0&&x-1<=w-1&&x-1>=0)
        cov00=outputData[(x-1)*rh+y-1];
    if(y-1<=h-1&&y-1>=0)
        cov10=outputData[x*rh+y-1];
    if(y-1<=h-1&&y-1>=0&&x+1<=w-1&&x+1>=0)
        cov20=outputData[(x+1)*rh+y-1];
    if(x-1<=w-1&&x-1>=0)
        cov01=outputData[(x-1)*rh+y];
    if(x+1<=w-1&&x+1>=0)
        cov21=outputData[(x+1)*rh+y];
    if(y+1<=h-1&&y+1>=0&&x-1<=w-1&&x-1>=0)
        cov02=outputData[(x-1)*rh+y+1];
    if(y+1<=h-1&&y+1>=0)
        cov12=outputData[x*rh+y+1];
    if(y+1<=h-1&&y+1>=0&&x+1<=w-1&&x+1>=0)
        cov22=outputData[(x+1)*rh+y+1];
    int horizontal_sobel=-cov00-2*cov10-cov20+cov02+2*cov12+cov22;
    int vertical_sobel=-cov00+cov20-2*cov01+2*cov21-cov02+cov22;
    return floor(qSqrt(qPow(horizontal_sobel,2)+qPow(vertical_sobel,2)));
}

int ImageProcess::gaussfiltering(int x, int y)
{
    int cov00=0,cov10=0,cov20=0,cov01=0,cov11=sobelImage[x*h+y],cov21=0,cov02=0,cov12=0,cov22=0;
    if(y-1<=h-1&&y-1>=0&&x-1<=w-1&&x-1>=0)
        cov00=sobelImage[(x-1)*h+y-1];
    if(y-1<=h-1&&y-1>=0)
        cov10=sobelImage[x*h+y-1];
    if(y-1<=h-1&&y-1>=0&&x+1<=w-1&&x+1>=0)
        cov20=sobelImage[(x+1)*h+y-1];
    if(x-1<=w-1&&x-1>=0)
        cov01=sobelImage[(x-1)*h+y];
    if(x+1<=w-1&&x+1>=0)
        cov21=sobelImage[(x+1)*h+y];
    if(y+1<=h-1&&y+1>=0&&x-1<=w-1&&x-1>=0)
        cov02=sobelImage[(x-1)*h+y+1];
    if(y+1<=h-1&&y+1>=0)
        cov12=sobelImage[x*h+y+1];
    if(y+1<=h-1&&y+1>=0&&x+1<=w-1&&x+1>=0)
        cov22=sobelImage[(x+1)*h+y+1];

    return floor((0.3679*cov00+0.6065*cov10+0.3679*cov20+0.6065*cov01+cov11+0.6065*cov21+0.3679*cov02+0.6065*cov12+0.3679*cov22)/4.8976);
}

void ImageProcess::save()
{

    if(savemodel!=nullptr)
    delete[] savemodel;
    savemodel=new int[rw*rh];
    for(int i=0;i<rw;i++)
        for(int j=0;j<rh;j++)
        savemodel[i*rh+j]=outputData[i*rh+j];

}

ImageProcess::~ImageProcess()
{
    if(sobelImage!=nullptr)
    delete [] sobelImage;
     if(outputData!=nullptr)
    delete [] outputData;
     if(inputData!=nullptr)
    delete [] inputData;
     if(savemodel!=nullptr)
    delete [] savemodel;
}

int ImageProcess::getPixel(int index)
{
    if(index<=255&&index>=0)return index;
    else if(index<0)return 0;
    else if(index>255)return 255;
}
