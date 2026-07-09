#include <QApplication>
#include <QFile>
#include <functional>
#include "bridge/api_bridge.h"
#include "ui/login_dialog.h"
#include "ui/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("图书管理系统");
    app.setApplicationVersion("1.0.0");
    app.setQuitOnLastWindowClosed(false);

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = styleFile.readAll();
        app.setStyleSheet(style);
        styleFile.close();
    }

    if (!ApiBridge::instance()->initialize(QApplication::applicationDirPath() + "/library.db")) {
        qCritical() << "Failed to initialize application";
        return -1;
    }

    std::function<void()> showMainWindow;
    std::function<void()> showLogin;

    showMainWindow = [&]() {
        auto* w = new MainWindow;
        w->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(w, &MainWindow::loggedOut, [&]() { showLogin(); });
        w->show();
    };

    showLogin = [&]() {
        LoginDialog dlg;
        if (dlg.exec() != QDialog::Accepted)
            QApplication::quit();
        else
            showMainWindow();
    };

    showLogin();
    return app.exec();
}
