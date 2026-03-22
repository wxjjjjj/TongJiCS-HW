#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyleSheet("QMessageBox QLabel { color: red; }");
    Widget w;
    w.show();
    return a.exec();
}
