#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QString>
#include <QVector>
#include <QObject>
#include <QRectF>
#include <QtMath>

// ============ 数据源类：封装数据管理、文件读取、随机生成 ============
class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = nullptr);

    // 从文件读取数据（支持 .txt .csv 等文本格式，数字以逗号/空格/换行分隔）
    bool loadFromFile(const QString &filePath);

    // 随机生成 count 个数据，范围 [min, max)
    void generateRandom(int count, double min = 0.0, double max = 100.0);

    // 手动设置数据
    void setData(const QVector<double> &newData);

    // 数据访问接口
    const QVector<double>& data() const { return m_data; }
    int count() const { return m_data.size(); }
    double sum() const;
    double maxValue() const;
    double minValue() const;

    // 错误信息
    QString lastError() const { return m_lastError; }

signals:
    // 数据变更信号 → 驱动图表动态刷新
    void dataChanged();

private:
    QVector<double> m_data;
    QString m_lastError;

    // 从文本中解析数值列表（按逗号、空格、换行切分）
    QVector<double> parseNumbers(const QString &text);
};

// ============ 抽象图表基类（继承 QWidget，具备绘图能力）============
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
    void paintEvent(QPaintEvent *) override;
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

// ============ 柱状图 ============
class Histogram : public Chart
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    Histogram(QString t, QColor c, QWidget *parent = nullptr);
    void draw(QPainter &painter) override;
};

// ============ 折线图 ============
class LineChart : public Chart
{
    Q_OBJECT
public:
    explicit LineChart(QWidget *parent = nullptr);
    LineChart(QString t, QColor c, QWidget *parent = nullptr);
    void draw(QPainter &painter) override;
};

// ============ 饼图 ============
class PieChart : public Chart
{
    Q_OBJECT
public:
    explicit PieChart(QWidget *parent = nullptr);
    PieChart(QString t, QColor c, QWidget *parent = nullptr);
    void draw(QPainter &painter) override;
};

#endif // CHART_H
