#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//创建一个对象,管理整个应用程序
    qApp->setStyleSheet("QMessageBox QLabel { color: red; }");
    Widget w;//创建主窗口,调用Widget构造函数--widget.cpp中
    w.show();//显示主窗口
    return a.exec();//进入事件循环,程序在这里等待用户操作
}
