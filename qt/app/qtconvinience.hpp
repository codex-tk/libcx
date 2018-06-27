#ifndef QTCONVINIENCE_HPP
#define QTCONVINIENCE_HPP

#include <QtWidgets/QLabel>
#include <cx/vision/image.hpp>

class QTConvinience
{
public:
    QTConvinience();

    static void bind( QLabel* lb , cx::vision::image& img );
};
/*
class CustomLabel : public QLabel
{
Q_OBJECT
signals:
    void mousePressed( const QPoint& );

public:
    CustomLabel( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    CustomLabel( const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0 );

    void mousePressEvent( QMouseEvent* ev );
};
*/
#endif // QTCONVINIENCE_HPP
