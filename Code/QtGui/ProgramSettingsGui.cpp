#include "ProgramSettingsGui.hpp"

#include "Utils/Console.hpp"

#include "QtUiConstants.hpp"
#include "QtUtil.hpp"

#include <QKeyEvent>

enum : unsigned int
{
	SMFileOption_LocalModFolders = 0,
	SMFileOption_WorkshopModFolders = 1,
	SMFileOption_UserItemFolders = 2
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

SettingsGeneralTab::SettingsGeneralTab(QWidget* parent)
	: QWidget(parent),
	m_pathGroupBox(new PathGroupBox("Path to Scrap Mechanic Folder", this)),
	m_bOpenLinksInSteam(new QCheckBox("Open Links in Steam", this)),
	m_mainLayout(new QVBoxLayout(this))
{
	m_mainLayout->setAlignment(Qt::AlignTop);
	m_mainLayout->addWidget(m_pathGroupBox);
	m_mainLayout->addWidget(m_bOpenLinksInSteam);
}

////////////////////SETTINGS PATHS TAB/////////////////////////

SettingsPathsTab::SettingsPathsTab(QWidget* parent)
	: QWidget(parent),
	m_changeDetector(),
	m_pathListView(new PathListViewWidget(this)),
	m_folderOptions(new QComboBox(this)),
	m_mainLayout(new QVBoxLayout(this))
{
	QObject::connect(
		m_folderOptions, &QComboBox::currentIndexChanged,
		this, &SettingsPathsTab::updateCurrentPathList);

	m_folderOptions->addItem("Local Mod Folders");
	m_folderOptions->addItem("Mod Folders");
	m_folderOptions->addItem("User Item Folders");

	m_mainLayout->addWidget(m_pathListView);
	m_mainLayout->addWidget(m_folderOptions, 0, Qt::AlignBottom);

	m_pathListView->setAddElementCallback(
		[](const QString& str) -> bool {
			DebugOutL("New element: ", str.toStdString());
			return true;
		}
	);

	m_pathListView->setRemoveElementCallback(
		[this](int idx) -> bool {
			DebugOutL("Remove element[", idx, "] = ", m_pathListView->m_pathStorage[idx].toStdString());
			return true;
		}
	);
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

////////////////////PROGRAM SETTINGS GUI////////////////////

ProgramSettingsGui::ProgramSettingsGui(QWidget* parent)
	: QDialog(parent),
	m_tabBar(new QTabWidget(this)),
	m_saveSettingsBtn(new QPushButton("Save Settings", this)),
	m_layout(new QVBoxLayout(this))
{
	this->setWindowTitle("Program Settings");

	m_saveSettingsBtn->setFixedHeight(m_saveSettingsBtn->fontMetrics().height() + g_windowPadding * 2);
	m_saveSettingsBtn->setFixedWidth(100);

	m_tabBar->addTab(new SettingsGeneralTab(m_tabBar), "General");
	m_tabBar->addTab(new SettingsPathsTab(m_tabBar), "Paths");

	m_layout->addWidget(m_tabBar);
	m_layout->addWidget(m_saveSettingsBtn, 0, Qt::AlignBottom | Qt::AlignRight);

	this->adjustSize();
}

void ProgramSettingsGui::keyPressEvent(QKeyEvent* event)
{
	if (event->key() != Qt::Key::Key_Escape)
		QDialog::keyPressEvent(event);
}