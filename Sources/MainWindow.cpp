#include "../Headers/MainWindow.h"
#include "ui_mainwindow.h"
#include <DGtal/base/Common.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString text = QString("Helloworld from DGtal (version %1)").arg(DGTAL_VERSION);

    ui->label->setText(text);
}

MainWindow::~MainWindow()
{
    delete ui;
}

