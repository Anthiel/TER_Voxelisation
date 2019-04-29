#include "mainwindow.h"
#include <QApplication>
#include <DGtal/base/Common.h>
int main(int argc, char *argv[])
{
    DGtal::trace.info() << "Helloworld from DGtal ";
    DGtal::trace.emphase() << "(version "<< DGTAL_VERSION << ")"<< std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
