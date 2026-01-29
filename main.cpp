#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置一些应用元数据
    QApplication::setApplicationName("MD Editor For AI - GUI");
    QApplication::setApplicationVersion("1.0");

    MainWindow w;
    w.show();
    return a.exec();
}
