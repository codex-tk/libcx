#ifndef FFTDIALOG_HPP
#define FFTDIALOG_HPP

#include <QDialog>
#include <cx/vision/image.hpp>

namespace Ui {
class FFTDialog;
}

class FFTDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FFTDialog(QWidget *parent , cx::vision::image& orig);
    ~FFTDialog();


protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    cx::vision::image _orig;
    Ui::FFTDialog *ui;
    cx::vision::image_base<double> _fft_re;
    cx::vision::image_base<double> _fft_im;
};

#endif // FFTDIALOG_HPP
