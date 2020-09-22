#include "MainWindow.hpp"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QLibraryInfo>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString translationsPath = QDir::currentPath() + "/translations/";
    QLocale locale = QLocale::system();

    QTranslator qtTranslator;
    if (qtTranslator.load(locale, "qt", "_", translationsPath)) {
        app.installTranslator(&qtTranslator);
    }
    else {
        QMessageBox::warning(nullptr, "Warning", "Qt translations file is not found.");
    }

    QTranslator translator;
    if (translator.load(locale, "FSTreeInfo", "_", translationsPath)) {
        app.installTranslator(&translator);
    }
    else {
        QMessageBox::warning(nullptr, "Warning", "File with Russian translation is not found. English will be used as interface language.");
    }

    MainWindow w;
    w.show();
    return app.exec();
}
