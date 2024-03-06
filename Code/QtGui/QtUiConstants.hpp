#pragma once

#if defined(_DEBUG) || defined(DEBUG)
#define SMCONV_DEBUG_POSTFIX " (DEBUG)"
#else
#define SMCONV_DEBUG_POSTFIX
#endif

#define SMCONV_MAIN_GUI_TITLE "SM Converter" SMCONV_DEBUG_POSTFIX

namespace
{
	constexpr int g_windowPadding = 10;
	constexpr int g_halfWindowPadding = g_windowPadding / 2;
	constexpr int g_commonWidgetHeight = 22;
	constexpr int g_comboBoxHeight = g_commonWidgetHeight + g_windowPadding + g_halfWindowPadding;

	constexpr int g_windowMargin = 10;
	constexpr int g_convertBtnWidth = 100;
	constexpr int g_utilBtnWidth = 75;
	constexpr int g_progressBarHeight = 15;
}