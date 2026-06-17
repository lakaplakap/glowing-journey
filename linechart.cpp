#include "linechart.h"

#include <QPainterPath>

// ==================== 折线图 LineChart 实现 ====================
LineChart::LineChart(QWidget *parent)
    : Chart("折线图", QColor::fromHsl(140, 200, 100), parent)
{}

LineChart::LineChart(QString t, QColor c, QWidget *parent)
    : Chart(t, c, parent)
{}

void LineChart::draw(QPainter &painter)
{
    drawTitle(painter);

    // 无数据时给出提示
    if (!m_dataSource || m_dataSource->count() == 0) {
        drawNoDataMessage(painter);
        return;
    }

    const QVector<double> &d = m_dataSource->data();
    int n = m_dataSource->count();
    double maxVal = m_dataSource->maxValue();
    if (maxVal <= 0) maxVal = 1;

    QRectF area = chartArea();
    area.setLeft(area.left() + 10);
    area.setTop(area.top() + 10);

    double left = area.left();
    double bottom = area.bottom();
    double drawWidth = area.width();
    double drawHeight = area.height();

    // 绘制网格和坐标轴
    drawGridLines(painter, area);
    drawAxes(painter, area, maxVal);

    // 动态计算点间距
    double stepX = drawWidth / (n > 1 ? double(n - 1) : 1);
    if (n == 1) stepX = 0;  // 单个点居中

    // 绘制折线和数据点
    painter.save();

    // 点标记
    QFont valFont = painter.font();
    valFont.setPointSize(9);
    painter.setFont(valFont);

    // 计算所有点的坐标
    QVector<QPointF> points;
    for (int i = 0; i < n; ++i) {
        double x = (n == 1) ? left + drawWidth / 2.0 : left + i * stepX;
        double y = bottom - (d[i] / maxVal) * drawHeight;
        points.append(QPointF(x, y));
    }

    // 绘制折线（带渐变透明度的区域填充）
    if (n >= 2) {
        // 折线
        painter.setPen(QPen(color, 2.5));
        painter.setBrush(Qt::NoBrush);
        for (int i = 0; i < n - 1; ++i) {
            painter.drawLine(points[i], points[i + 1]);
        }

        // 半透明区域填充
        QPainterPath areaPath;
        areaPath.moveTo(points[0].x(), bottom);
        for (int i = 0; i < n; ++i) {
            areaPath.lineTo(points[i]);
        }
        areaPath.lineTo(points[n - 1].x(), bottom);
        areaPath.closeSubpath();

        QColor fillColor = color;
        fillColor.setAlpha(40);
        painter.setPen(Qt::NoPen);
        painter.setBrush(fillColor);
        painter.drawPath(areaPath);
    }

    // 绘制数据点
    painter.setPen(QPen(color.darker(120), 1.5));
    painter.setBrush(color.lighter(160));
    for (int i = 0; i < n; ++i) {
        painter.drawEllipse(points[i], 5, 5);

        // 数值标签（偏移到点上方避免遮挡）
        painter.setPen(QColor(0x33, 0x33, 0x33));
        QRectF valRect(points[i].x() - 25, points[i].y() - 24, 50, 18);
        painter.drawText(valRect, Qt::AlignHCenter | Qt::AlignBottom,
                         QString::number(d[i], 'f', 1));
        painter.setPen(QPen(color.darker(120), 1.5));
    }

    // X 轴标签
    painter.setPen(QColor(0x55, 0x55, 0x55));
    for (int i = 0; i < n; ++i) {
        QRectF xLabelRect(points[i].x() - 15, bottom + 4, 30, 18);
        painter.drawText(xLabelRect, Qt::AlignHCenter | Qt::AlignTop,
                         QString::number(i + 1));
    }

    painter.restore();
}
