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
#include "winDark.h"

#include "Utils/WinInclude.hpp"

#include <QStyleFactory>
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QStyle>

namespace winDark {

/**
 * @brief isDarkTheme
 * @return bool
 */
bool isDarkTheme()
{
#ifdef Q_OS_WIN
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
					   QSettings::NativeFormat);
	if(settings.value("AppsUseLightTheme")==0){
		return true;
	}
#endif
	return false;
}

/**
 * @brief setDark_qApp
 * @details use this after creating QApplication
 *          if(winDark::isDarkTheme()) winDark::setDark_qApp();
 *
 */
void setDark_qApp()
{
#ifdef Q_OS_WIN
		qApp->setStyle(QStyleFactory::create("Fusion"));
		QPalette darkPalette;
		const QColor darkColor = QColor(25,25,25);
		const QColor disabledColor = QColor(127,127,127);
		darkPalette.setColor(QPalette::Window, darkColor);
		darkPalette.setColor(QPalette::WindowText, Qt::white);
		darkPalette.setColor(QPalette::Base, QColor(18,18,18));
		darkPalette.setColor(QPalette::AlternateBase, darkColor);
		darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
		darkPalette.setColor(QPalette::ToolTipText, Qt::white);
		darkPalette.setColor(QPalette::Text, Qt::white);
		darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
		darkPalette.setColor(QPalette::Button, darkColor);
		darkPalette.setColor(QPalette::ButtonText, Qt::white);
		darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
		darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
		darkPalette.setColor(QPalette::BrightText, Qt::red);
		darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
		darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
		darkPalette.setColor(QPalette::HighlightedText, Qt::black);
		darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);
		darkPalette.setColor(QPalette::Dark, QColor(53, 53, 53));
		darkPalette.setColor(QPalette::Accent, QColor(255, 0, 255));
		qApp->setPalette(darkPalette);
		qApp->setStyleSheet(
			"QComboBox QAbstractItemView { border: 1px solid #353535; } "
			"QMenuBar::item:selected { background-color: rgb(50, 50, 50); } "
			"QMenu { background-color: rgb(25, 25, 25); border: 1px solid #353535; } "
			"QMenu::item { padding-left: 10px; padding-top: 5px; padding-bottom: 5px; padding-right: 10px } "
			"QMenu::item:selected { background-color: rgb(50, 50, 50); }"
			"QMenu::separator { height: 1px; margin-left: 5px; margin-right: 5px; background: #353535; } "
		);
#endif
}

/**
 * @brief setDark_Titlebar
 * @param hwnd
 * @details Use for each window like
 *    if(winDark::isDarkTheme()) winDark::setDark_Titlebar(reinterpret_cast<HWND>(winId()));
 */
void setDark_Titlebar(HWND hwnd)
{
#ifdef Q_OS_WIN
	using dwm_set_window_attribute_t = HRESULT(__stdcall*)(
		HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);

	HMODULE v_dwm_mod = LoadLibraryA("dwmapi.dll");
	if (v_dwm_mod)
	{
		dwm_set_window_attribute_t v_set_wnd_attrib;
		v_set_wnd_attrib = reinterpret_cast<dwm_set_window_attribute_t>(GetProcAddress(v_dwm_mod, "DwmSetWindowAttribute"));
		if (v_set_wnd_attrib)
		{
			//DWMWA_USE_IMMERSIVE_DARK_MODE 20
			BOOL v_is_dark = true;
			v_set_wnd_attrib(hwnd, 20, &v_is_dark, sizeof(v_is_dark));
		}

		FreeLibrary(v_dwm_mod);
	}
#endif
}

}
