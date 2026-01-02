#include "CustomGameSelector.hpp"

#include "ObjectDatabase/Mods/CustomGameMod.hpp"
#include "Converter/ConvertSettings.hpp"

CustomGameSelector::CustomGameSelector(QWidget* parent, const std::wstring& path)
	: NamedComboBox(parent)
{
	this->setTitle("Custom Game Content");

	m_comboBox->addItem("None");
	for (CustomGame* v_cur_game : SMMod::GetCustomGames())
		m_comboBox->addItem(QString::fromStdWString(v_cur_game->GetName()));

	CustomGame* v_cur_cg = SMMod::GetCustomGameFromPath(path);
	m_comboBox->setCurrentIndex((v_cur_cg != nullptr) ? int(v_cur_cg->m_id + 1) : 0);
}

int CustomGameSelector::getCustomGameIndex() const
{
	return m_comboBox->currentIndex() - 1;
}