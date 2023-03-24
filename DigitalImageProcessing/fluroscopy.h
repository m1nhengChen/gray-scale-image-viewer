#ifndef FLUROSCOPY_H
#define FLUROSCOPY_H
#include"mainwindow.h"
#include <QDialog>

namespace Ui {
class Fluroscopy;
}

class Fluroscopy : public QDialog
{
    Q_OBJECT
public:
    explicit Fluroscopy(QWidget *parent = nullptr);
    MainWindow *mainWindow;
    ~Fluroscopy();
    void setParameter(int p ,int w);

private slots:
    void on_pushButtonConfirm_clicked();

    void on_pushButtonBack_clicked();

private:
    Ui::Fluroscopy *ui;
    int pos,wid;
};

#endif // FLUROSCOPY_H
