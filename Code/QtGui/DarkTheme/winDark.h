/**
 *
 * Respect Windows Dark/Light themes for QT windows
 * Naveen K <prime@envyen.com>
 *
 * Based on https://github.com/statiolake/neovim-qt/commit/da8eaba7f0e38b6b51f3bacd02a8cc2d1f7a34d8
 * Warning: this is not a proper solution, and Windows api can change !!
 * Stylesheet based on https://forum.qt.io/post/523388
 *
 **/
#ifndef WINDARK_H
#define WINDARK_H

#include <QStyleFactory>
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QWidget>

namespace winDark {

void setDark_qApp();
void setDark_Titlebar(HWND hwnd);
bool isDarkTheme();

} // ns

#endif // WINDARK_H
