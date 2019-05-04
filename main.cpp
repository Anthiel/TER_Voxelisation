#include "mainwindow.h"
#include <QApplication>
#include <DGtal/base/Common.h>

int main(int argc, char *argv[])
{
    DGtal::trace.emphase() << "Running DGtal (version " << DGTAL_VERSION << ")" << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
