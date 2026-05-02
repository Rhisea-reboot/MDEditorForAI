#include "MainWindow.h"
#include "SplashScreen.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("MD Editor For AI - GUI");
    QApplication::setApplicationVersion("1.0");

    // Apply a global stylesheet for a cleaner look
    a.setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
    )");

    MainWindow *w = new MainWindow;

    SplashScreen splash;
    splash.startAnimation();

    QObject::connect(&splash, &SplashScreen::finished, [w]() {
        w->show();
        w->raise();
        w->activateWindow();
    });

    return a.exec();
}
