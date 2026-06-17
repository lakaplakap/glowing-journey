#include "chart.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <cmath>

// ==================== DataSource 实现 ====================
DataSource::DataSource(QObject *parent)
    : QObject(parent)
{}

bool DataSource::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("无法打开文件: %1").arg(filePath);
        return false;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    m_data = parseNumbers(content);
    if (m_data.isEmpty()) {
        m_lastError = "文件中未找到有效数值数据";
        return false;
    }

    m_lastError.clear();
    emit dataChanged();  // 通知图表动态刷新
    return true;
}

void DataSource::generateRandom(int count, double min, double max)
{
    m_data.clear();
    m_data.reserve(count);

    auto *rng = QRandomGenerator::global();
    for (int i = 0; i < count; ++i) {
        double value = min + rng->generateDouble() * (max - min);
        // 保留一位小数
        value = qRound(value * 10.0) / 10.0;
        m_data.append(value);
    }

    emit dataChanged();  // 通知图表动态刷新
}

void DataSource::setData(const QVector<double> &newData)
{
    m_data = newData;
    emit dataChanged();  // 通知图表动态刷新
}

double DataSource::sum() const
{
    double s = 0;
    for (double v : m_data) s += v;
    return s;
}

double DataSource::maxValue() const
{
    if (m_data.isEmpty()) return 0;
    double m = m_data[0];
    for (double v : m_data)
        if (v > m) m = v;
    return m;
}

double DataSource::minValue() const
{
    if (m_data.isEmpty()) return 0;
    double m = m_data[0];
    for (double v : m_data)
        if (v < m) m = v;
    return m;
}

QVector<double> DataSource::parseNumbers(const QString &text)
{
    QVector<double> result;
    // 按逗号、空格、换行等分隔符拆分成 token
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QStringList tokens = text.split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
#else
    QStringList tokens = text.split(QRegularExpression("[,\\s]+"), QString::SkipEmptyParts);
#endif

    for (const QString &token : tokens) {
        bool ok = false;
        double value = token.toDouble(&ok);
        if (ok) {
            result.append(value);
        }
    }
    return result;
}

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

// ==================== 饼图 PieChart 实现 ====================
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
        QString label = QString("数据 %1: %2 (%3%)")
                            .arg(i + 1)
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
