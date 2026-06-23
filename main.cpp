#include <QApplication>
#include "mainwindow.h"

//全局样式表
static const char *APP_STYLE = R"(
    QMainWindow {
        background-color: #F5F6FA;
    }
    QPushButton {
        padding: 8px 18px;
        border-radius: 6px;
        border: 1px solid #C8CCD4;
        background-color: #FFFFFF;
        color: #333333;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton:hover {
        background-color: #E8EDF5;
        border-color: #A0A8B8;
    }
    QPushButton:pressed {
        background-color: #D0D7E5;
    }
    QPushButton#btnHistogram[active="true"],
    QPushButton#btnLineChart[active="true"],
    QPushButton#btnPieChart[active="true"] {
        background-color: #3B7CBF;
        color: #FFFFFF;
        border-color: #2E6096;
    }
    QStatusBar {
        background-color: #EEF1F7;
        border-top: 1px solid #D8DCE4;
        font-size: 12px;
        color: #1A56DB;
        padding: 3px;
    }
    QStatusBar QLabel{
        color: #1A56DB;
        font-size: 12px;
    }
    QGroupBox {
        font-weight: bold;
        color: #444444;
        border: 1px solid #D8DCE4;
        border-radius: 6px;
        margin-top: 8px;
        padding-top: 14px;
        background-color: #FFFFFF;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        left: 12px;
        padding: 0 6px;
    }
)";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(APP_STYLE);

    MainWindow w;
    w.show();

    return a.exec();
}
