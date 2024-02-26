#include "FilterSettingsGui.hpp"

#include "ObjectDatabase\UserDataReaders\FilterSettingsData.hpp"

#include "Utils/Console.hpp"

inline static void SetBit(std::uint8_t& byte_ref, std::uint8_t value, bool is_enabled) noexcept
{
	if (is_enabled)
		byte_ref |= value;
	else
		byte_ref &= ~value;
}

inline static void UpdateUserDataFilter(std::uint8_t value, bool is_enabled) noexcept
{
	SetBit(FilterSettingsData::UserDataFilter, value, is_enabled);
}

FilterSettingsGui::FilterSettingsGui(QWidget* parent)
	: QDialog(parent),
	m_tileSizeFilterCopy(FilterSettingsData::TileSizeFilter),
	m_userDataFilterCopy(FilterSettingsData::UserDataFilter),
	m_commonFilters(this),
	m_bShowWorkshopItems(new QCheckBox("Show Workshop Items", &m_commonFilters)),
	m_bShowGameItems(new QCheckBox("Show Game Items", &m_commonFilters)),
	m_bShowUserItems(new QCheckBox("Show User Items", &m_commonFilters)),
	m_tileSizeFilter(this),
	m_layout(new QVBoxLayout(this))
{
	this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	this->setWindowTitle("Filter Settings");

	m_bShowWorkshopItems->setChecked(FilterSettingsData::UserDataFilter & UserDataFilter_WorkshopItems);
	m_bShowGameItems->setChecked(FilterSettingsData::UserDataFilter & UserDataFilter_GameItems);
	m_bShowUserItems->setChecked(FilterSettingsData::UserDataFilter & UserDataFilter_UserItems);

	QObject::connect(m_bShowWorkshopItems, &QCheckBox::stateChanged, this,
		[this]() { UpdateUserDataFilter(UserDataFilter_WorkshopItems, m_bShowWorkshopItems->isChecked()); });
	QObject::connect(m_bShowGameItems, &QCheckBox::stateChanged, this,
		[this]() { UpdateUserDataFilter(UserDataFilter_GameItems, m_bShowGameItems->isChecked()); });
	QObject::connect(m_bShowUserItems, &QCheckBox::stateChanged, this,
		[this]() { UpdateUserDataFilter(UserDataFilter_UserItems, m_bShowUserItems->isChecked()); });

	m_commonFilters.setTitle("Common Filters");
	m_commonFilters.m_layout->setAlignment(Qt::AlignTop);
	m_commonFilters.m_layout->addWidget(m_bShowWorkshopItems);
	m_commonFilters.m_layout->addWidget(m_bShowGameItems);
	m_commonFilters.m_layout->addWidget(m_bShowUserItems);

	m_tileSizeFilter.setTitle("Tile Size Filter");
	m_tileSizeFilter.m_comboBox->addItems({
		"Show All Tiles",
		"Show Small Tiles",
		"Show Medium Tiles",
		"Show Large Tiles",
		"Show Extra Large Tiles"
	});
	m_tileSizeFilter.m_comboBox->setCurrentIndex(this->getSelectedFilterIndex());
	QObject::connect(m_tileSizeFilter.m_comboBox, &QComboBox::currentIndexChanged,
		this, &FilterSettingsGui::tileSizeFilterChanged);

	m_layout->addWidget(&m_commonFilters);
	m_layout->addWidget(&m_tileSizeFilter);

	this->adjustSize();
}

int FilterSettingsGui::getSelectedFilterIndex()
{
	switch (FilterSettingsData::TileSizeFilter)
	{
	case TileSizeFilter_Any:        return 0;
	case TileSizeFilter_Small:      return 1;
	case TileSizeFilter_Medium:     return 2;
	case TileSizeFilter_Large:      return 3;
	case TileSizeFilter_ExtraLarge: return 4;
	}

	return 0;
}

void FilterSettingsGui::tileSizeFilterChanged() const
{
	const static std::uint8_t v_filters[] =
	{
		TileSizeFilter_Any,
		TileSizeFilter_Small,
		TileSizeFilter_Medium,
		TileSizeFilter_Large,
		TileSizeFilter_ExtraLarge
	};

	FilterSettingsData::TileSizeFilter = v_filters[
		m_tileSizeFilter.m_comboBox->currentIndex()];
}

bool FilterSettingsGui::filtersChanged() const
{
	return m_bShouldReload;
}

void FilterSettingsGui::closeEvent(QCloseEvent* event)
{
	m_bShouldReload =
		(FilterSettingsData::UserDataFilter != m_userDataFilterCopy) ||
		(FilterSettingsData::TileSizeFilter != m_tileSizeFilterCopy);
}