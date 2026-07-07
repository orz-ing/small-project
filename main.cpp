#include <QApplication>
#include <QFile>
#include "main_window.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 加载全局样式
    QFile styleFile(":/res/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
