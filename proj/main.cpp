#include "catalog_wdg.h"
#include <QApplication>
#include <QStyleFactory>
#include <QSettings>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(true);

    // Ini-files are used instead of native OS formats(
    QSettings::setDefaultFormat(QSettings::IniFormat);

    if (QStyleFactory::keys().contains(QLatin1Literal("Fusion")))
        app.setStyle(QStyleFactory::create(QLatin1Literal("Fusion")));

    auto wdg = std::make_unique<CatalogWidget>();
    wdg->readSettings();
    wdg->show();

    const auto retCode = app.exec();

    wdg->writeSettings();

    return retCode;
}
