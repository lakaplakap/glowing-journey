#include "piechart.h"

//饼图 PieChart 实现
PieChart::PieChart(QWidget *parent)
    : Chart("饼图", QColor(0xEE, 0xAA, 0x00), parent)
{}

PieChart::PieChart(QString t, QColor c, QWidget *parent)
    : Chart(t, c, parent)
{}

void PieChart::draw(QPainter &painter)
{
    drawTitle(painter);

    // 无数据时给出提示
    if (!m_dataSource || m_dataSource->count() == 0) {
        drawNoDataMessage(painter);
        return;
    }

    const QVector<double> &d = m_dataSource->data();
    int n = m_dataSource->count();
    double sum = m_dataSource->sum();

    if (sum == 0) {
        drawNoDataMessage(painter, "数据总和为零，无法绘制饼图");
        return;
    }

    QRectF area = chartArea();
    area.setTop(area.top() + 5);

    // 将区域分为左侧饼图区和右侧图例区
    double legendWidth = 200;
    double pieAreaWidth = area.width() - legendWidth - 20;
    double pieAreaHeight = area.height();

    // 饼图圆心和半径
    double cx = area.left() + pieAreaWidth / 2.0;
    double cy = area.top() + pieAreaHeight / 2.0;
    double r = qMin(pieAreaWidth, pieAreaHeight) / 2.0 * 0.78;

    // 绘制扇形
    double startAngle = 90.0 * 16;  // Qt 0° = 3 点钟, +90° = 12 点钟方向起算
    painter.save();

    for (int i = 0; i < n; ++i) {
        double ratio = d[i] / sum;
        double spanAngle = ratio * 360.0 * 16;

        QColor sliceCol = sliceColor(i, n);

        // 扇形主体（白色边框分隔扇区）
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(sliceCol);
        painter.drawPie(QRectF(cx - r, cy - r, r * 2, r * 2),
                        int(startAngle), int(-spanAngle));

        // ---- 在扇形内部绘制百分比 ----
        if (ratio >= 0.04) {  // 小于 4% 的扇区空间不足以显示标签，跳过
            // 扇形中点角度（1/16° 单位 → 度 → 弧度）
            double midAngleDeg = (startAngle - spanAngle / 2.0) / 16.0;
            double midRad = qDegreesToRadians(midAngleDeg);
            double labelR = r * 0.62;  // 标签放置在半径 62% 处
            double labelX = cx + labelR * cos(midRad);
            double labelY = cy - labelR * sin(midRad);  // Qt Y 轴向下，因此取负

            painter.save();
            QFont pctFont = painter.font();
            pctFont.setPointSize(11);
            pctFont.setBold(true);
            painter.setFont(pctFont);

            QString pctText = QString("%1%").arg(ratio * 100, 0, 'f', 1);

            // 先绘制深色阴影偏移，增强对比度
            painter.setPen(QColor(0, 0, 0, 120));
            painter.drawText(QRectF(labelX - 40, labelY + 1 - 12, 80, 24),
                             Qt::AlignCenter, pctText);
            // 再绘制白色文字主体
            painter.setPen(Qt::white);
            painter.drawText(QRectF(labelX - 40, labelY - 12, 80, 24),
                             Qt::AlignCenter, pctText);
            painter.restore();
        }

        startAngle -= spanAngle;  // 顺时针：下一扇区起点 = 当前终点
    }

    // 绘制饼图中心装饰圆
    painter.setBrush(Qt::white);
    painter.setPen(QPen(QColor(0xCC, 0xCC, 0xCC), 1));
    painter.drawEllipse(QPointF(cx, cy), r * 0.18, r * 0.18);

    painter.restore();

    // ---- 右侧图例 ----
    double legendX = area.left() + pieAreaWidth + 10;
    double legendY = area.top() + 20;

    painter.save();
    QFont legendFont = painter.font();
    legendFont.setPointSize(10);
    painter.setFont(legendFont);

    // 图例标题
    QFont legendTitleFont = legendFont;
    legendTitleFont.setBold(true);
    legendTitleFont.setPointSize(11);
    painter.setFont(legendTitleFont);
    painter.setPen(QColor(0x33, 0x33, 0x33));
    painter.drawText(QRectF(legendX, legendY, legendWidth - 10, 22),
                     Qt::AlignLeft | Qt::AlignVCenter, "图例");

    legendY += 28;

    // 图例项
    painter.setFont(legendFont);
    double swatchSize = 14;
    for (int i = 0; i < n; ++i) {
        QColor sliceCol = sliceColor(i, n);
        double ratio = d[i] / sum;

        // 色块
        painter.setPen(QPen(sliceCol.darker(120), 1));
        painter.setBrush(sliceCol);
        painter.drawRoundedRect(QRectF(legendX, legendY + 2, swatchSize, swatchSize), 2, 2);

        // 标签
        painter.setPen(QColor(0x33, 0x33, 0x33));
        QString itemName;
        if (m_dataSource->hasLabels() && i < m_dataSource->labels().size()) {
            itemName = m_dataSource->labels()[i];
            // 超长标签截断
            QFontMetrics fm(painter.font());
            int maxLabelWidth = int(legendWidth - swatchSize - 80);
            if (fm.horizontalAdvance(itemName) > maxLabelWidth) {
                itemName = fm.elidedText(itemName, Qt::ElideRight, maxLabelWidth);
            }
        } else {
            itemName = QString("数据 %1").arg(i + 1);
        }
        QString label = QString("%1: %2 (%3%)")
                            .arg(itemName)
                            .arg(d[i], 0, 'f', 1)
                            .arg(ratio * 100, 0, 'f', 1);
        painter.drawText(QRectF(legendX + swatchSize + 6, legendY, legendWidth - swatchSize - 16, 18),
                         Qt::AlignLeft | Qt::AlignVCenter, label);

        legendY += 24;
    }

    // 总计
    legendY += 6;
    painter.setPen(QPen(QColor(0xAA, 0xAA, 0xAA), 1));
    painter.drawLine(QPointF(legendX, legendY), QPointF(legendX + legendWidth - 10, legendY));
    legendY += 8;
    QFont sumFont = legendFont;
    sumFont.setBold(true);
    painter.setFont(sumFont);
    painter.setPen(QColor(0x33, 0x33, 0x33));
    painter.drawText(QRectF(legendX, legendY, legendWidth - 10, 20),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString("合计: %1").arg(sum, 0, 'f', 1));

    painter.restore();
}
