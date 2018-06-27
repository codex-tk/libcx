#ifndef BINARYSAMPLEDIALOG_H
#define BINARYSAMPLEDIALOG_H

#include <QDialog>

#include <cx/vision/image_proc.hpp>


#include "app.hpp"


namespace Ui {
class BinarySampleDialog;
}

class BinarySampleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BinarySampleDialog(QWidget *parent = 0);
    ~BinarySampleDialog();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_BinarySampleDialog_accepted();

private:
    Ui::BinarySampleDialog *ui;
    cx::vision::image _image;
    cx::vision::image _binary;
};

#endif // BINARYSAMPLEDIALOG_H
