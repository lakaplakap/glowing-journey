#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include <QString>
#include <QVector>

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

#endif // DATASOURCE_H
