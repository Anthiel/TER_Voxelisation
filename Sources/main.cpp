#include <iostream>
#include <DGtal/base/Common.h>
#include <QApplication>
#include "../Headers/MainWindow.h"


using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    cout << "Hello World!" << endl;
    DGtal::trace.info() << "Helloworld from DGtal ";
    DGtal::trace.emphase() << "(version "<< DGTAL_VERSION << ")"<< std::endl;
    return a.exec();
}
