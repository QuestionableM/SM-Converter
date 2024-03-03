#pragma once

#include "QtGui/CustomWidgets/AlignedGroupBox.hpp"

#include <QCheckBox>
#include <QDialog>

class FilterSettingsGui : public QDialog
{
public:
	FilterSettingsGui(QWidget* parent = nullptr);
	~FilterSettingsGui() = default;

	static int getSelectedFilterIndex();

	void tileSizeFilterChanged() const;
	bool filtersChanged() const;

	void closeEvent(QCloseEvent*) override;


public:
	std::uint8_t m_tileSizeFilterCopy;
	std::uint8_t m_userDataFilterCopy;
	bool m_bShouldReload;

	AlignedGroupBox m_commonFilters;
	QCheckBox* m_bShowWorkshopItems;
	QCheckBox* m_bShowGameItems;
	QCheckBox* m_bShowUserItems;

	NamedComboBox m_tileSizeFilter;

	QVBoxLayout* m_layout;
};