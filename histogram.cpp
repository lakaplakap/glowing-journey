#include "histogram.h"

// ==================== 柱状图 Histogram 实现 ====================
Histogram::Histogram(QWidget *parent)
    : Chart("柱状图", QColor::fromHsl(0, 200, 120), parent)
{}

Histogram::Histogram(QString t, QColor c, QWidget *parent)
    : Chart(t, c, parent)
{}

void Histogram::draw(QPainter &painter)
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
    if (maxVal <= 0) maxVal = 1;  // 避免零刻度

    QRectF area = chartArea();
    // 为 Y 轴标签留出空间
    area.setLeft(area.left() + 10);
    area.setTop(area.top() + 10);

    double left = area.left();
    double bottom = area.bottom();
    double drawWidth = area.width();
    double drawHeight = area.height();

    // 绘制网格和坐标轴
    drawGridLines(painter, area);
    drawAxes(painter, area, maxVal);

    // 动态计算柱宽和间距
    int maxBarWidth = 60;
    int minBarWidth = 8;
    double totalGap = drawWidth * 0.15;  // 15% 作为总间距
    double barWidth = (drawWidth - totalGap) / n;
    if (barWidth > maxBarWidth) barWidth = maxBarWidth;
    if (barWidth < minBarWidth) barWidth = minBarWidth;
    double gap = (drawWidth - barWidth * n) / (n + 1);

    // 绘制柱状图
    painter.save();
    QFont valFont = painter.font();
    valFont.setPointSize(9);
    painter.setFont(valFont);

    for (int i = 0; i < n; ++i) {
        double x = left + gap + i * (barWidth + gap);
        double h = (d[i] / maxVal) * drawHeight;
        double y = bottom - h;

        // 渐变色：根据值的大小调整亮度
        double ratio = d[i] / maxVal;
        QColor barColor = color.lighter(200 - int(ratio * 120));

        // 绘制柱子
        painter.setPen(QPen(color.darker(120), 1));
        painter.setBrush(barColor);
        painter.drawRoundedRect(QRectF(x, y, barWidth, h), 3, 3);

        // 柱顶数值标签
        painter.setPen(QColor(0x33, 0x33, 0x33));
        QRectF valRect(x - 5, y - 22, barWidth + 10, 18);
        painter.drawText(valRect, Qt::AlignHCenter | Qt::AlignBottom,
                         QString::number(d[i], 'f', 1));

        // X 轴标签（索引号）
        QRectF xLabelRect(x, bottom + 4, barWidth, 18);
        painter.drawText(xLabelRect, Qt::AlignHCenter | Qt::AlignTop,
                         QString::number(i + 1));
    }
    painter.restore();
}
