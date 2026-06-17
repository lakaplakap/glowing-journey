#ifndef PIECHART_H
#define PIECHART_H

#include "chart.h"

// ============ 饼图 ============
class PieChart : public Chart
{
    Q_OBJECT
public:
    explicit PieChart(QWidget *parent = nullptr);
    PieChart(QString t, QColor c, QWidget *parent = nullptr);
    void draw(QPainter &painter) override;
};

#endif // PIECHART_H
