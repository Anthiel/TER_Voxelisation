#ifndef CONVERTER_H
#define CONVERTER_H

#include "mainwindow.h"

class Converter
{
private:
    QString fileName;

public:
    Converter(QMainWindow* parent, const QString fileName);
    void convertToDV();
    void convertTo3D();
    void convertToVOL();
    void convertToRAW();
};

#endif // CONVERTER_H
