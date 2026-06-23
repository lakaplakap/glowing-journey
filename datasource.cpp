#include "datasource.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <cmath>

//DataSource 实现
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
    m_labels.clear();
    m_hasLabels = false;

    if (m_data.isEmpty()) {
        m_lastError = "文件中未找到有效数值数据";
        return false;
    }

    m_lastError.clear();
    emit dataChanged();  // 通知图表动态刷新
    return true;
}

bool DataSource::loadFromLabeledFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("无法打开文件: %1").arg(filePath);
        return false;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // 按行拆分
    QStringList lines = content.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
    if (lines.isEmpty()) {
        m_lastError = "文件为空";
        return false;
    }

    QVector<QString> labels;
    QVector<double> values;

    // 检查第一行是否为标题头（第一个字段非数值）
    int startIndex = 0;
    {
        QString firstLine = lines[0].trimmed();
        // 尝试按逗号、制表符或多个空格拆分第一行
        QStringList firstTokens;
        if (firstLine.contains(',')) {
            firstTokens = firstLine.split(',', Qt::SkipEmptyParts);
        } else if (firstLine.contains('\t')) {
            firstTokens = firstLine.split('\t', Qt::SkipEmptyParts);
        } else {
            firstTokens = firstLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        }
        if (firstTokens.size() >= 2) {
            // 检查第二个 token 是否为数值
            bool secondIsNumber = false;
            firstTokens[1].trimmed().toDouble(&secondIsNumber);
            if (!secondIsNumber) {
                // 第二个字段不是数值 → 视为标题头，跳过
                startIndex = 1;
            }
        }
    }

    // 逐行解析标签和数值
    for (int i = startIndex; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        QString label;
        double value = 0;
        bool ok = false;

        // 优先按逗号拆分
        if (line.contains(',')) {
            QStringList parts = line.split(',', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                label = parts[0].trimmed();
                value = parts[1].trimmed().toDouble(&ok);
            }
        }
        // 其次按制表符拆分
        else if (line.contains('\t')) {
            QStringList parts = line.split('\t', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                label = parts[0].trimmed();
                value = parts[1].trimmed().toDouble(&ok);
            }
        }
        // 最后尝试按空格拆分：第一个 token 为标签，最后一个 token 为数值
        else {
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                // 最后一个 token 作为数值
                value = parts.last().trimmed().toDouble(&ok);
                // 除最后一个外的所有 token 拼接为标签
                parts.removeLast();
                label = parts.join(' ').trimmed();
            }
        }

        if (ok && !label.isEmpty()) {
            labels.append(label);
            values.append(value);
        }
    }

    if (values.isEmpty()) {
        m_lastError = "文件中未找到有效的\"标签,数值\"格式数据";
        m_data.clear();
        m_labels.clear();
        m_hasLabels = false;
        return false;
    }

    m_data = values;
    m_labels = labels;
    m_hasLabels = true;
    m_lastError.clear();
    emit dataChanged();
    return true;
}

void DataSource::generateRandom(int count, double min, double max)
{
    m_data.clear();
    m_data.reserve(count);
    m_labels.clear();
    m_hasLabels = false;

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
    m_labels.clear();
    m_hasLabels = false;
    emit dataChanged();  // 通知图表动态刷新
}

void DataSource::setLabeledData(const QVector<QString> &labels, const QVector<double> &values)
{
    if (labels.size() != values.size()) {
        m_lastError = "标签数量与数值数量不一致";
        return;
    }
    m_data = values;
    m_labels = labels;
    m_hasLabels = true;
    m_lastError.clear();
    emit dataChanged();
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

    for (const QString &token : std::as_const(tokens)) {
        bool ok = false;
        double value = token.toDouble(&ok);
        if (ok) {
            result.append(value);
        }
    }
    return result;
}
