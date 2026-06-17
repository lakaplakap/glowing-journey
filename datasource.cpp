#include "datasource.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRandomGenerator>
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
