#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <vector>
#include <QtMath>
#include<QDebug>
typedef std::vector<int>	RgbVector;
typedef std::vector<RgbVector>	RgbVector2D;
class ImageProcess
{
public:
    ImageProcess();
    void setBmpData(QImage image);
    QImage setRawData(unsigned short* input,int width,int height);
    void geometryTransform(double sx,double sy,int rx,int ry,double r);//几何变换
    QImage geometryTransform_r(double sx,double sy,int rx,int ry,double r,int pos,int wid);//几何变换
    void init_pixel_table();
    void reverse(bool hasprocess);//黑白翻转
    void turnAround(bool hasprocess);//左右翻转
    QImage restore();//恢复图像
    void clear();
    void copy(int ex_rh,int ex_rw);
    QImage grayScaleTransform(int pos,int wid,bool hasprocess);//灰度变换
    void imageEnhanced(bool hasprocess);//图像增强
    QImage flouroscopyImage(int pos,int wid);//透视图像浏览器
    int laplace4Cov(int x,int y);
    int laplace8Cov(int x,int y);
    int laplace8Cov_r(int x,int y);
    int sobel(int x,int y);
    int sobel_r(int x,int y);
    int gaussfiltering(int x,int y);
    void save();
     ~ImageProcess();
     int w,h,rw,rh,highest_intensity;
     int getPixel(int index);

private:
     int * sobelImage;
     QVector<QRgb>pixelTable;
     RgbVector2D *data;//图像存储数据类型
     int *inputData;
     int *outputData;//图像存储数据类型
     int *savemodel;

};

#endif // IMAGEPROCESS_H
