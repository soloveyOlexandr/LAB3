#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //створює і показує головне вікно нашої програми
    MainWindow w;
    w.show();
    return a.exec();
}
