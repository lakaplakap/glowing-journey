#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "chart.h"

//柱状图
class Histogram : public Chart
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    Histogram(QString t, QColor c, QWidget *parent = nullptr);
    virtual void draw(QPainter &painter);
};

#endif
