#ifndef LINECHART_H
#define LINECHART_H

#include "chart.h"

// ============ 折线图 ============
class LineChart : public Chart
{
    Q_OBJECT
public:
    explicit LineChart(QWidget *parent = nullptr);
    LineChart(QString t, QColor c, QWidget *parent = nullptr);
    void draw(QPainter &painter) override;
};

#endif // LINECHART_H
