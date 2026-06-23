#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include <QString>
#include <QVector>

//数据源类：封装数据管理、文件读取、随机生成
class DataSource : public QObject
{
    Q_OBJECT
private:
    QVector<double> m_data;
    QVector<QString> m_labels;  // 数据标签（如 "苹果"、"香蕉"），与 m_data 一一对应
    bool m_hasLabels = false;
    QString m_lastError;

    // 从文本中解析数值列表（按逗号、空格、换行切分）
    QVector<double> parseNumbers(const QString &text);

public:
    explicit DataSource(QObject *parent = nullptr);

    // 从文件读取数据（支持 .txt .csv 等文本格式，数字以逗号/空格/换行分隔）
    bool loadFromFile(const QString &filePath);

    // 从带标签的文件读取数据（每行格式: "标签,数值" 或 "标签 数值"）
    bool loadFromLabeledFile(const QString &filePath);

    // 随机生成 count 个数据，范围 [min, max)
    void generateRandom(int count, double min = 0.0, double max = 100.0);

    // 手动设置数据
    void setData(const QVector<double> &newData);

    // 手动设置带标签的数据
    void setLabeledData(const QVector<QString> &labels, const QVector<double> &values);

    // 数据访问接口
    const QVector<double>& data() const {
        return m_data;
    }
    const QVector<QString>& labels() const {
        return m_labels;
    }
    bool hasLabels() const {
        return m_hasLabels;
    }
    int count() const {
        return m_data.size();
    }
    double sum() const;
    double maxValue() const;
    double minValue() const;

    // 错误信息
    QString lastError() const {
        return m_lastError;
    }

signals:
    // 数据变更信号 → 驱动图表动态刷新
    void dataChanged();

};

#endif
