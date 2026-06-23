#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QList>

#include "datasource.h"
#include "chart.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadFile();
    void onLoadLabeledFile();
    void onRandomGenerate();
    void onClearData();
    void onChartTypeClicked();
    void refreshStatusBar();

private:
    void setupChart();
    void switchChart(Chart *newChart, QPushButton *activeBtn);
    void highlightChartButton(QPushButton *active);

    Ui::MainWindow *ui;
    DataSource *m_dataSource;
    Chart *m_chartView = nullptr;
    QList<QPushButton*> m_chartButtons;

    // Status bar labels
    QLabel *m_statusCount;
    QLabel *m_statusSum;
    QLabel *m_statusMin;
    QLabel *m_statusMax;
};

#endif