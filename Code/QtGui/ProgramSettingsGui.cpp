#include "ProgramSettingsGui.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"
#include "Utils/Console.hpp"
#include "Utils/File.hpp"

#include "QtUiConstants.hpp"
#include "QtUtil.hpp"

#include <QMessageBox>
#include <QKeyEvent>

enum : unsigned int
{
	SMFileOption_LocalModFolders = 0,
	SMFileOption_WorkshopModFolders = 1,
	SMFileOption_UserItemFolders = 2
};

//Translates SMFileOption flags to SettingsChangeDetector options
const unsigned char g_file_option_translation[] =
{
	SettingsChangeDetector_LocalModList,
	SettingsChangeDetector_WorkshopModList,
	SettingsChangeDetector_UserItemFolder
};

////////////////////////PATH GROUP BOX///////////////////

PathGroupBox::PathGroupBox(const QString& title, QWidget* parent)
	: AlignedGroupBox(parent, title),
	m_path(new QLineEdit(this)),
	m_selectPathButton(new QPushButton("...", this)),
	m_hLayout(new QHBoxLayout(this))
{
	QObject::connect(m_selectPathButton, &QPushButton::pressed, this,
		[this]() -> void {
			QString	v_selected_directory;
			if (!QtUtil::fileDialog(this, "Select a path to Scrap Mechanic directory",
				QFileDialog::FileMode::Directory, v_selected_directory))
			{
				return;
			}

			m_path->setText(v_selected_directory);
		}
	);
	
	m_hLayout->addWidget(m_path);
	m_hLayout->addWidget(m_selectPathButton);

	const QMargins v_margins = m_layout->contentsMargins();
	m_layout->setContentsMargins(v_margins.left(), 0, v_margins.right(), v_margins.bottom());
	m_layout->addLayout(m_hLayout);
}

////////////////////SETTINGS GENERAL TAB//////////////////////

SettingsGeneralTab::SettingsGeneralTab(
	SettingsChangeDetector& detector,
	QWidget* parent)
	: SettingsTabBase(parent),
	m_changeDetector(detector),
	m_pathGroupBox(new PathGroupBox("Path to Scrap Mechanic Folder", this)),
	m_bOpenLinksInSteam(new QCheckBox("Open Links in Steam", this)),
	m_mainLayout(new QVBoxLayout(this))
{
	m_mainLayout->setAlignment(Qt::AlignTop);
	m_mainLayout->addWidget(m_pathGroupBox);
	m_mainLayout->addWidget(m_bOpenLinksInSteam);

	m_bOpenLinksInSteam->setChecked(DatabaseConfig::OpenLinksInSteam);
	m_pathGroupBox->m_path->setText(
		QString::fromStdWString(DatabaseConfig::GamePath));

	QObject::connect(
		m_bOpenLinksInSteam, &QCheckBox::stateChanged,
		this, &SettingsGeneralTab::onCheckboxUpdate);

	QObject::connect(
		m_pathGroupBox->m_path, &QLineEdit::textChanged,
		this, &SettingsGeneralTab::onGamePathUpdate);
}

void SettingsGeneralTab::onCheckboxUpdate()
{
	m_changeDetector.m_openLinksInSteam = m_bOpenLinksInSteam->isChecked();
	m_changeDetector.UpdateChange(SettingsChangeDetector_OpenLinksInSteam);

	this->onSettingChanged();
}

void SettingsGeneralTab::onGamePathUpdate()
{
	m_changeDetector.m_gamePath = m_pathGroupBox->m_path->text().toStdWString();
	m_changeDetector.UpdateChange(SettingsChangeDetector_GamePath);

	this->onSettingChanged();
}


////////////////////SETTINGS PATHS TAB/////////////////////////

SettingsPathsTab::SettingsPathsTab(
	SettingsChangeDetector& detector, QWidget* parent)
	: SettingsTabBase(parent),
	m_changeDetector(detector),
	m_pathListView(new PathListViewWidget(this)),
	m_folderOptions(new QComboBox(this)),
	m_mainLayout(new QVBoxLayout(this))
{
	QObject::connect(
		m_folderOptions, &QComboBox::currentIndexChanged,
		this, &SettingsPathsTab::updateCurrentPathList);

	m_pathListView->setAddElementCallback(
		std::bind(&SettingsPathsTab::onElementAdded, this, std::placeholders::_1, std::placeholders::_2));
	m_pathListView->setRemoveElementCallback(
		std::bind(&SettingsPathsTab::onElementRemove, this, std::placeholders::_1));

	m_folderOptions->addItem("Local Mod Folders");
	m_folderOptions->addItem("Mod Folders");
	m_folderOptions->addItem("User Item Folders");

	m_mainLayout->addWidget(m_pathListView);
	m_mainLayout->addWidget(m_folderOptions, 0, Qt::AlignBottom);
}

void SettingsPathsTab::updateCurrentPathList()
{
	switch (m_folderOptions->currentIndex())
	{
	case SMFileOption_LocalModFolders:
		this->updatePathListView(m_changeDetector.m_localModList);
		break;
	case SMFileOption_WorkshopModFolders:
		this->updatePathListView(m_changeDetector.m_workshopModList);
		break;
	case SMFileOption_UserItemFolders:
		this->updatePathListView(m_changeDetector.m_userItemFolders);
		break;
	}
}

bool SettingsPathsTab::onElementAdded(int idx, const QString& str)
{
	const std::wstring v_new_path = str.toStdWString();
	if (!File::Exists(v_new_path))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "The specified path doesn't exist!");
		return false;
	}

	if (!File::IsDirectory(v_new_path))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "The specified path must lead to a directory");
		return false;
	}

	bool v_success = false;

	const int v_cur_option = m_folderOptions->currentIndex();
	switch (v_cur_option)
	{
	case SMFileOption_LocalModFolders:
		v_success = DatabaseConfig::AddToStrVec(
			m_changeDetector.m_localModList, m_changeDetector.m_modListMap, v_new_path);
		break;
	case SMFileOption_WorkshopModFolders:
		v_success = DatabaseConfig::AddToStrVec(
			m_changeDetector.m_workshopModList, m_changeDetector.m_modListMap, v_new_path);
		break;
	case SMFileOption_UserItemFolders:
		v_success = DatabaseConfig::AddToStrMap(
			m_changeDetector.m_userItemFolders, v_new_path);
		break;
	default:
		return false;
	}

	if (!v_success)
	{
		QtUtil::warningWithSound(
			this, "Invalid Path", "The specified path already exists in the list!");
		return false;
	}

	m_changeDetector.UpdateChange(g_file_option_translation[v_cur_option]);
	this->onSettingChanged();

	return true;
}

bool SettingsPathsTab::onElementRemove(int idx)
{
	const int v_cur_option = m_folderOptions->currentIndex();
	switch (v_cur_option)
	{
	case SMFileOption_LocalModFolders:
		SettingsChangeDetector::RemoveFromCheckedVec(
			m_changeDetector.m_localModList, m_changeDetector.m_modListMap, std::size_t(idx));
		break;
	case SMFileOption_WorkshopModFolders:
		SettingsChangeDetector::RemoveFromCheckedVec(
			m_changeDetector.m_workshopModList, m_changeDetector.m_modListMap, std::size_t(idx));
		break;
	case SMFileOption_UserItemFolders:
		SettingsChangeDetector::RemoveFromMap(
			m_changeDetector.m_userItemFolders, m_pathListView->m_pathStorage[idx].toStdWString());
		break;
	default:
		return false;
	}

	m_changeDetector.UpdateChange(g_file_option_translation[v_cur_option]);
	this->onSettingChanged();

	return true;
}

////////////////////PROGRAM SETTINGS GUI////////////////////

ProgramSettingsGui::ProgramSettingsGui(QWidget* parent)
	: QDialog(parent),
	m_changeDetector(),
	m_generalTab(new SettingsGeneralTab(m_changeDetector, this)),
	m_pathsTab(new SettingsPathsTab(m_changeDetector, this)),
	m_tabBar(new QTabWidget(this)),
	m_saveSettingsBtn(new QPushButton("Save Settings", this)),
	m_layout(new QVBoxLayout(this)),
	m_bReloadObjectDatabase(false),
	m_bReloadUserObjects(false)
{
	this->setWindowTitle("Program Settings");
	this->setMinimumWidth(450);
	this->setMinimumHeight(300);

	QObject::connect(
		m_pathsTab, &SettingsPathsTab::onSettingChanged,
		this, &ProgramSettingsGui::updateSaveButton);

	QObject::connect(
		m_generalTab, &SettingsGeneralTab::onSettingChanged,
		this, &ProgramSettingsGui::updateSaveButton);

	QObject::connect(
		m_saveSettingsBtn, &QPushButton::pressed,
		this, &ProgramSettingsGui::saveButtonClick);

	m_saveSettingsBtn->setFixedHeight(m_saveSettingsBtn->fontMetrics().height() + g_windowPadding * 2);
	m_saveSettingsBtn->setFixedWidth(100);
	m_saveSettingsBtn->setEnabled(false);

	m_tabBar->addTab(m_generalTab, "General");
	m_tabBar->addTab(m_pathsTab, "Paths");

	m_layout->addWidget(m_tabBar);
	m_layout->addWidget(m_saveSettingsBtn, 0, Qt::AlignBottom | Qt::AlignRight);

	this->adjustSize();
}

void ProgramSettingsGui::updateSaveButton()
{
	m_saveSettingsBtn->setEnabled(m_changeDetector.HasAnyChanges());
}

void ProgramSettingsGui::saveButtonClick()
{
	if (m_generalTab->m_pathGroupBox->m_path->text().length() == 0)
	{
		QtUtil::warningWithSound(this,
			"Empty Path", "The path to the game must be filled in");
		return;
	}

	const std::wstring v_game_path = m_generalTab->m_pathGroupBox->m_path->text().toStdWString();
	if (!File::Exists(v_game_path))
	{
		QtUtil::warningWithSound(this,
			"Invalid Game Path", "The specified path to the game doesn't exist");
		return;
	}

	if (!File::IsDirectory(v_game_path))
	{
		QtUtil::warningWithSound(this,
			"Invalid Game Path", "The path must lead to the root directory of the game");
		return;
	}


	m_bReloadObjectDatabase |= m_changeDetector.IsAnyBitSet(SettingsChangeDetector_GamePath);
	m_bReloadUserObjects |= m_changeDetector.IsAnyBitSet(
		SettingsChangeDetector_LocalModList |
		SettingsChangeDetector_WorkshopModList |
		SettingsChangeDetector_UserItemFolder);

	m_changeDetector.ApplyChanges();
	DatabaseConfig::SaveConfig();

	m_saveSettingsBtn->setEnabled(false);
}

bool ProgramSettingsGui::shouldReloadObjectDatabase() const
{
	return m_bReloadObjectDatabase;
}

bool ProgramSettingsGui::shouldReloadUserObjects() const
{
	return m_bReloadUserObjects;
}

void ProgramSettingsGui::keyPressEvent(QKeyEvent* event)
{
	if (event->key() != Qt::Key::Key_Escape)
		QDialog::keyPressEvent(event);
}

void ProgramSettingsGui::closeEvent(QCloseEvent* event)
{
	if (!m_saveSettingsBtn->isEnabled())
		return;

	QMessageBox v_msg_box(this);
	v_msg_box.setWindowTitle("Unsaved Changes");
	v_msg_box.setIcon(QMessageBox::Question);
	v_msg_box.setText("Do you want to leave the settings without saving?\n\nAll unsaved changes will be lost!");
	v_msg_box.setStandardButtons(
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

	if (v_msg_box.exec() != QMessageBox::StandardButton::Yes)
		event->ignore();
}