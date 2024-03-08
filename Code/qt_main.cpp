#include <QtWidgets/QApplication>

#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/DatabaseConfig.hpp"
#include "Utils/Console.hpp"

#include "QtGui/DarkTheme/winDark.h"
#include "QtGui/MainGui.hpp"

int main(int argc, char* argv[])
{
    CreateDebugConsole(L"SM Converter Debug Console");

    QApplication a(argc, argv);

    DatabaseLoader::InitializeDatabase();
    if (DatabaseConfig::IsDarkMode)
        winDark::setDark_qApp();

    MainGui w;
    w.show();

    return a.exec();
}