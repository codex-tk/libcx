#ifndef HISTOGRAMDIALOG_HPP
#define HISTOGRAMDIALOG_HPP

#include <QDialog>
#include <cx/vision/image.hpp>

namespace Ui {
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent  , const cx::vision::image& img );
    ~HistogramDialog();


protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();

private:
    Ui::HistogramDialog *ui;
    cx::vision::image _image;
};

#endif // HISTOGRAMDIALOG_HPP
