/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QGroupBox *toolGroup;
    QHBoxLayout *toolLayout;
    QLabel *charttypelabel;
    QPushButton *btnHistogram;
    QPushButton *btnLineChart;
    QPushButton *btnPieChart;
    QFrame *separator;
    QLabel *dataOpLabel;
    QPushButton *btnLoad;
    QPushButton *btnLoadLabeled;
    QPushButton *btnRandom;
    QPushButton *btnClear;
    QSpacerItem *horizontalSpacer;
    QWidget *chartContainer;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 750);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(8);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(12, 10, 12, 8);
        toolGroup = new QGroupBox(centralWidget);
        toolGroup->setObjectName("toolGroup");
        toolLayout = new QHBoxLayout(toolGroup);
        toolLayout->setSpacing(10);
        toolLayout->setObjectName("toolLayout");
        charttypelabel = new QLabel(toolGroup);
        charttypelabel->setObjectName("charttypelabel");
        charttypelabel->setStyleSheet(QString::fromUtf8("font-weight:bold; color:#555;"));

        toolLayout->addWidget(charttypelabel);

        btnHistogram = new QPushButton(toolGroup);
        btnHistogram->setObjectName("btnHistogram");

        toolLayout->addWidget(btnHistogram);

        btnLineChart = new QPushButton(toolGroup);
        btnLineChart->setObjectName("btnLineChart");

        toolLayout->addWidget(btnLineChart);

        btnPieChart = new QPushButton(toolGroup);
        btnPieChart->setObjectName("btnPieChart");

        toolLayout->addWidget(btnPieChart);

        separator = new QFrame(toolGroup);
        separator->setObjectName("separator");
        separator->setStyleSheet(QString::fromUtf8("color:#D8DCE4;"));
        separator->setFrameShape(QFrame::Shape::VLine);

        toolLayout->addWidget(separator);

        dataOpLabel = new QLabel(toolGroup);
        dataOpLabel->setObjectName("dataOpLabel");
        dataOpLabel->setStyleSheet(QString::fromUtf8("font-weight:bold; color:#555;"));

        toolLayout->addWidget(dataOpLabel);

        btnLoad = new QPushButton(toolGroup);
        btnLoad->setObjectName("btnLoad");

        toolLayout->addWidget(btnLoad);

        btnLoadLabeled = new QPushButton(toolGroup);
        btnLoadLabeled->setObjectName("btnLoadLabeled");

        toolLayout->addWidget(btnLoadLabeled);

        btnRandom = new QPushButton(toolGroup);
        btnRandom->setObjectName("btnRandom");

        toolLayout->addWidget(btnRandom);

        btnClear = new QPushButton(toolGroup);
        btnClear->setObjectName("btnClear");

        toolLayout->addWidget(btnClear);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        toolLayout->addItem(horizontalSpacer);


        mainLayout->addWidget(toolGroup);

        chartContainer = new QWidget(centralWidget);
        chartContainer->setObjectName("chartContainer");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(chartContainer->sizePolicy().hasHeightForWidth());
        chartContainer->setSizePolicy(sizePolicy);

        mainLayout->addWidget(chartContainer);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\351\235\242\345\220\221\345\257\271\350\261\241\345\233\276\350\241\250\347\224\237\346\210\220\347\263\273\347\273\237\357\274\210Qt Designer\347\211\210\357\274\211", nullptr));
        toolGroup->setTitle(QCoreApplication::translate("MainWindow", "\345\233\276\350\241\250\346\223\215\344\275\234", nullptr));
        charttypelabel->setText(QCoreApplication::translate("MainWindow", "\345\233\276\350\241\250\347\261\273\345\236\213:", nullptr));
        btnHistogram->setText(QCoreApplication::translate("MainWindow", "\346\237\261\347\212\266\345\233\276", nullptr));
        btnLineChart->setText(QCoreApplication::translate("MainWindow", "\346\212\230\347\272\277\345\233\276", nullptr));
        btnPieChart->setText(QCoreApplication::translate("MainWindow", "\351\245\274\345\233\276", nullptr));
        dataOpLabel->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\223\215\344\275\234:", nullptr));
        btnLoad->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\346\226\207\344\273\266", nullptr));
        btnLoadLabeled->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\346\240\207\347\255\276\346\226\207\344\273\266", nullptr));
        btnRandom->setText(QCoreApplication::translate("MainWindow", " \351\232\217\346\234\272\347\224\237\346\210\220", nullptr));
        btnClear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\225\260\346\215\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
