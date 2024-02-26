#include <QtWidgets/QApplication>

#include "Utils/Console.hpp"
#include "QtGui/MainGui.hpp"

int main(int argc, char* argv[])
{
    CreateDebugConsole(L"SM Converter Debug Console");

    QApplication a(argc, argv);
    MainGui w;
    w.show();
    return a.exec();
}