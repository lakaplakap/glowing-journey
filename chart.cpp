#include "chart.h"

#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPainterPath>

// ==================== Chart 基类实现 ====================
Chart::Chart(QWidget *parent)
    : QWidget(parent), title("默认图表"), color(Qt::blue), m_dataSource(nullptr)
{
    setMinimumSize(700, 500);
}

Chart::Chart(QString t, QColor c, QWidget *parent)
    : QWidget(parent), title(t), color(c), m_dataSource(nullptr)
{
    setMinimumSize(700, 500);
}

void Chart::setTitle(QString t)
{
    title = t;
    update();
}

void Chart::setColor(QColor c)
{
    color = c;
    update();
}

void Chart::setDataSource(DataSource *ds)
{
    // 断开旧数据源连接
    if (m_dataSource) {
        disconnect(m_dataSource, &DataSource::dataChanged, this, nullptr);
    }

    m_dataSource = ds;

    // 连接新数据源：数据变更 → 自动重绘图表
    if (m_dataSource) {
        connect(m_dataSource, &DataSource::dataChanged, this, [this]() {
            update();
        });
    }

    update();  // 立即重绘以反映新数据
}

void Chart::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    draw(painter);
}

// ---- 布局辅助 ----
QRectF Chart::chartArea() const
{
    return QRectF(m_margin, m_margin,
                  width() - 2.0 * m_margin,
                  height() - 2.0 * m_margin);
}

// ---- 公用绘制辅助 ----
void Chart::drawTitle(QPainter &p)
{
    p.save();
    QFont font = p.font();
    font.setPointSize(16);
    font.setBold(true);
    p.setFont(font);
    p.setPen(QColor(0x33, 0x33, 0x33));

    QRectF titleRect(0, 8, width(), 40);
    p.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, title);
    p.restore();
}

void Chart::drawNoDataMessage(QPainter &p, const QString &msg)
{
    p.save();
    QFont font = p.font();
    font.setPointSize(14);
    p.setFont(font);
    p.setPen(QColor(0x99, 0x99, 0x99));

    QString text = msg.isEmpty() ? "暂无数据，请加载文件或随机生成" : msg;
    p.drawText(chartArea(), Qt::AlignCenter, text);
    p.restore();
}

void Chart::drawAxes(QPainter &p, const QRectF &area, double maxVal, int tickCount)
{
    p.save();
    p.setPen(QPen(QColor(0x55, 0x55, 0x55), 1.5));

    double left = area.left();
    double right = area.right();
    double top = area.top();
    double bottom = area.bottom();

    // Y 轴（左侧竖线）
    p.drawLine(QPointF(left, top), QPointF(left, bottom));
    // X 轴（底部横线）
    p.drawLine(QPointF(left, bottom), QPointF(right, bottom));

    // Y 轴刻度与标签
    QFont tickFont = p.font();
    tickFont.setPointSize(9);
    p.setFont(tickFont);

    double yRange = bottom - top;
    for (int i = 0; i <= tickCount; ++i) {
        double val = maxVal * i / tickCount;
        double y = bottom - yRange * i / tickCount;

        // 刻度短线
        p.drawLine(QPointF(left - 5, y), QPointF(left, y));

        // 刻度标签
        QRectF labelRect(left - 55, y - 10, 48, 20);
        p.setPen(QColor(0x55, 0x55, 0x55));
        p.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter,
                   QString::number(val, 'f', 1));
    }
    p.restore();
}

void Chart::drawGridLines(QPainter &p, const QRectF &area, int tickCount)
{
    p.save();
    QPen gridPen(QColor(0xDD, 0xDD, 0xDD), 1, Qt::DashLine);
    p.setPen(gridPen);

    double left = area.left();
    double right = area.right();
    double top = area.top();
    double bottom = area.bottom();
    double yRange = bottom - top;

    for (int i = 0; i <= tickCount; ++i) {
        double y = bottom - yRange * i / tickCount;
        p.drawLine(QPointF(left, y), QPointF(right, y));
    }
    p.restore();
}

// ---- 饼图配色辅助：HSL 色相均匀分布 ----
QColor Chart::sliceColor(int index, int total)
{
    // 色相均匀分布，饱和度和亮度固定，产生视觉区分度高的配色
    int hue = (index * 360 / total) % 360;
    return QColor::fromHsl(hue, 180, 140);
}
