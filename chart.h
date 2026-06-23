#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QPainter>
#include <QString>
#include <QRectF>
#include <QtMath>

#include "datasource.h"

//抽象图表基类（继承 QWidget，具备绘图能力）
class Chart : public QWidget
{
    Q_OBJECT
protected:
    QString title;
    QColor color;
    DataSource *m_dataSource;  // 数据源指针（由外部注入）
    int m_margin = 70;         // 图表区域四周留白

public:
    explicit Chart(QWidget *parent = nullptr);
    Chart(QString t, QColor c, QWidget *parent = nullptr);

    void setTitle(QString t);
    void setColor(QColor c);

    // 注入数据源 → 自动连接 dataChanged 信号实现动态刷新
    void setDataSource(DataSource *ds);

    // 纯虚函数：子类实现具体图表绘制
    virtual void draw(QPainter &painter) = 0;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual ~Chart() = default;

    // ---- 布局辅助 ----
    QRectF chartArea() const;

    // ---- 公用绘制辅助（子类可在 draw() 中调用）----
    void drawTitle(QPainter &p);
    void drawNoDataMessage(QPainter &p, const QString &msg = QString());
    void drawAxes(QPainter &p, const QRectF &area, double maxVal, int tickCount = 5);
    void drawGridLines(QPainter &p, const QRectF &area, int tickCount = 5);

    // ---- 饼图配色辅助：根据索引生成 HSL 颜色 ----
    static QColor sliceColor(int index, int total);
};

#endif
