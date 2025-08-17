#include "MainGui.hpp"

#include "ObjectDatabase/UserDataReaders/UserObjectFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/BlueprintFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/UserCharacterReader.hpp"
#include "ObjectDatabase/UserDataReaders/WorldFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/TileFolderReader.hpp"
#include "ObjectDatabase/GroundTextureDatabase.hpp"
#include "ObjectDatabase/KeywordReplacer.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/DatabaseConfig.hpp"
#include "ObjectDatabase/ProgCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Converter/BlueprintConverter/BlueprintConverter.hpp"
#include "Converter/WorldConverter/WorldConverter.hpp"
#include "Converter/TileConverter/TileConverter.hpp"
#include "Converter/ConvertSettings.hpp"

#include "ConvSettings/BlueprintConvertSettingsGui.hpp"
#include "ConvSettings/TileConverterSettingsGui.hpp"
#include "ProgramSettingsGui.hpp"
#include "FilterSettingsGui.hpp"
#include "AboutProgramGui.hpp"
#include "MainGuiMenuBar.hpp"

#include "QtUiConstants.hpp"
#include "Utils/File.hpp"
#include "QtUtil.hpp"

#include "../resource.h"

#include <QDesktopServices>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QLineEdit>
#include <QMenuBar>

MainGui::MainGui(QWidget* parent) :
	QMainWindow(parent),
	m_contextMenu(new ObjectListContextMenu(this)),
	m_menuBar(new MainGuiMenuBar(this)),
	m_objectPathButton(new QPushButton("...", this)),
	m_objectPath(new QLineEdit(this)),
	m_searchFilterButton(new QPushButton("Filter", this)),
	m_searchBox(new QLineEdit(this)),
	m_dbProgressBar(new QProgressBar(this)),
	m_dbProgressStatus(new LabelTimer("NONE", this)),
	m_dbLoaderThread(nullptr),
	m_objectList(new UserObjectListView(this)),
	m_objectLoaderStatus(new LabelTimer("NONE", this)),
	m_userObjectLoaderThread(nullptr),
	m_converterTypeBox(new QComboBox(this)),
	m_convertButton(new QPushButton("Convert", this)),
	m_converterThread(nullptr),
	m_converterReturnCode()
{
	g_mainGuiPtr = this;

	this->setWindowIcon(QIcon("icon.ico"));

	this->initializeUI();
	this->connectEvents();
	this->initializeDatabase();
}

MainGui::~MainGui()
{
	g_mainGuiPtr = nullptr;
}

void MainGui::initializeUI()
{
	this->setWindowTitle(SMCONV_MAIN_GUI_TITLE);
	this->setMinimumSize(450, 350);
	this->resize(450, 350);

	m_objectList->setContextMenuStrip(m_contextMenu);

	m_objectPath->setPlaceholderText("Object path");
	m_searchBox->setPlaceholderText("Search");

	m_dbProgressBar->setTextVisible(false);

	m_converterTypeBox->addItem("Blueprint Converter (.json, .blueprint)");
	m_converterTypeBox->addItem("Tile Converter (.tile)");

//#if defined(SMCONV_ENABLE_WORLD_CONVERTER) //Mostly finished
	m_converterTypeBox->addItem("World Converter (.json, .world)");
//#endif

#if defined(SMCONV_ENABLE_CHARACTER_CONVERTER) //WIP
	m_converterTypeBox->addItem("Character Converter");
#endif

	m_converterTypeBox->setCurrentIndex(0);
}

void MainGui::connectEvents()
{
	QObject::connect(m_objectLoaderStatus->m_timer, &QTimer::timeout, this, &MainGui::updateUserObjectStatusCallback);
	QObject::connect(m_dbProgressStatus->m_timer, &QTimer::timeout, this, &MainGui::updateDbProgressCallback);
	
	QObject::connect(m_menuBar->m_reloadUserObjectsAction, &QAction::triggered, this, &MainGui::loadUserObjects);
	QObject::connect(m_menuBar->m_reloadObjectDatabaseAction, &QAction::triggered, this,
		[this]() -> void { loadObjectDatabase(false); });
	QObject::connect(m_menuBar->m_openBlueprintOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::BlueprintOutputFolder, "blueprint"); });
	QObject::connect(m_menuBar->m_openWorldOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::WorldOutputFolder, "world"); });
	QObject::connect(m_menuBar->m_openTileOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::TileOutputFolder, "tile"); });
	QObject::connect(m_menuBar->m_aboutProgramAction, &QAction::triggered, this,
		[this]() -> void { AboutProgramGui(this).exec(); });
	QObject::connect(
		m_menuBar->m_openProgramSettingsAction, &QAction::triggered,
		this, &MainGui::openProgramSettings);
	
	QObject::connect(m_objectList, &UserObjectListView::selectedIndexChanged, this, &MainGui::selectedObjectChanged);
	QObject::connect(m_converterTypeBox, &QComboBox::currentIndexChanged, this, &MainGui::converterTypeChanged);
	
	QObject::connect(m_objectPath, &QLineEdit::textChanged, this, &MainGui::objectPathTextChanged);
	QObject::connect(m_searchBox, &QLineEdit::textChanged, this, &MainGui::searchTextChanged);

	QObject::connect(m_searchFilterButton, &QPushButton::clicked, this, &MainGui::openFilterSettings);
	QObject::connect(m_convertButton, &QPushButton::clicked, this, &MainGui::convert);
	QObject::connect(m_objectPathButton, &QPushButton::clicked, this,
		[this]() -> void {
			QString v_selected_file;
			if (!QtUtil::fileDialog(this, "Select a Blueprint",
				QFileDialog::FileMode::ExistingFile, v_selected_file))
			{
				return;
			}

			m_objectPath->setText(v_selected_file);
		}
	);

	QObject::connect(
		m_contextMenu->m_openInSteamWorkshopAction, &QAction::triggered,
		this, &MainGui::openItemInSteamWorkshop);

	QObject::connect(
		m_contextMenu->m_openInExplorerAction, &QAction::triggered,
		this, &MainGui::openItemInExplorer);

	QObject::connect(
		m_contextMenu->m_openCreatorProfile, &QAction::triggered,
		this, &MainGui::openCreatorProfile);

	QObject::connect(
		m_contextMenu->m_copyWorkshopIdAction, &QAction::triggered,
		this, &MainGui::copyItemWorkshopId);

	QObject::connect(
		m_contextMenu->m_copyCreatorIdAction, &QAction::triggered,
		this, &MainGui::copyItemAuthorId);
	
	QObject::connect(
		m_contextMenu->m_copyUuidAction, &QAction::triggered,
		this, &MainGui::copyItemUuid);

	QObject::connect(
		m_contextMenu->m_showObjectInfoAction, &QAction::triggered,
		this, &MainGui::openObjectInfoGui);
}

void MainGui::initializeDatabase()
{
	this->loadObjectDatabase(true);
	this->loadUserObjects();

	this->updateSearchTextBox();
}

void MainGui::resizeEvent(QResizeEvent* event)
{
	this->blockSignals(true);

	m_menuBar->resize(this->width(), 22);

	const int v_util_btn_start_x = this->width() - g_windowMargin - g_utilBtnWidth;
	const int v_edit_box_width = this->width() - g_utilBtnWidth - g_windowMargin * 3;
	const int v_convert_btn_height = m_convertButton->fontMetrics().height() + g_windowMargin * 2;

	m_objectPathButton->setGeometry(
		v_util_btn_start_x,
		m_menuBar->y() + m_menuBar->height() + g_windowMargin,
		g_utilBtnWidth,
		g_commonWidgetHeight);

	m_objectPath->setGeometry(
		g_windowMargin,
		m_objectPathButton->y(),
		v_edit_box_width,
		g_commonWidgetHeight);

	m_searchFilterButton->setGeometry(
		v_util_btn_start_x,
		m_objectPathButton->y() + m_objectPathButton->height() + g_windowMargin,
		g_utilBtnWidth,
		g_commonWidgetHeight);

	m_searchBox->setGeometry(
		g_windowMargin,
		m_searchFilterButton->y(),
		v_edit_box_width,
		g_commonWidgetHeight);

	m_convertButton->setGeometry(
		this->width() - g_windowMargin - g_convertBtnWidth,
		this->height() - g_windowMargin - v_convert_btn_height,
		g_convertBtnWidth,
		v_convert_btn_height);

	m_converterTypeBox->setGeometry(
		g_windowMargin,
		m_convertButton->y() - g_windowMargin - g_commonWidgetHeight,
		this->width() - g_windowMargin * 2,
		g_commonWidgetHeight);

	m_dbProgressBar->setGeometry(
		g_windowMargin,
		this->height() - g_progressBarHeight - g_windowMargin,
		this->width() - m_convertButton->width() - g_windowMargin * 3,
		g_progressBarHeight);

	m_dbProgressStatus->setGeometry(
		g_windowMargin,
		m_convertButton->y(),
		m_dbProgressBar->width(),
		m_dbProgressStatus->fontMetrics().height());

	const int v_object_list_y = m_searchBox->y() + m_searchBox->height() + g_windowMargin;

	m_objectList->setGeometry(
		g_windowMargin,
		v_object_list_y,
		this->width() - g_windowMargin * 2,
		(m_converterTypeBox->y() - g_windowMargin) - v_object_list_y);

	this->updateUserObjectStatusPosition();

	this->blockSignals(false);
}

void MainGui::closeEvent(QCloseEvent* event)
{
	if (!m_converterThread || !m_converterThread->isRunning())
		return;

	QMessageBox v_mbox(
		QMessageBox::Question,
		"Closing",
		"Are you sure you want to close the program while an item is being converted?\n\nClosing the program might produce a corrupt model!",
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
		this
	);

	if (v_mbox.exec() != QMessageBox::StandardButton::Yes)
		event->ignore();
}

void MainGui::updateUserObjectStatusCallback()
{
	if (!m_objectLoaderStatus->isVisible())
		return;

	QString v_status_text;
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		v_status_text = QString("Loading blueprints... (%1)")
			.arg(BlueprintFolderReader::ObjectCounter.load());
		break;
	case ConvType_TileConverter:
		v_status_text = QString("Loading Tiles... (%1)")
			.arg(TileFolderReader::ObjectCounter.load());
		break;
	case ConvType_WorldConverter:
		v_status_text = QString("Loading Worlds... (%1)")
			.arg(WorldFolderReader::ObjectCounter.load());
		break;
	case ConvType_CharacterConverter:
		v_status_text = QString("Loading Characters... (%1)")
			.arg(UserCharacterReader::ObjectCounter.load());
		break;
	}

	this->updateUserObjectStatusText(v_status_text);
}

void MainGui::userObjectsLoadedCallback()
{
	m_objectLoaderStatus->m_timer->stop();
	m_objectLoaderStatus->setVisible(false);

	if (m_userObjectLoaderThread)
	{
		delete m_userObjectLoaderThread;
		m_userObjectLoaderThread = nullptr;
	}

	this->updateUIState(this->isGameDatabaseLoaded(), true, true);
	this->updateSearchResults(0);
}

void MainGui::converterFinishedCallback()
{
	m_dbProgressStatus->m_timer->stop();
	m_dbProgressBar->setValue(m_dbProgressBar->maximum());

	if (m_converterThread)
	{
		delete m_converterThread;
		m_converterThread = nullptr;
	}

	std::lock_guard v_lock_g(m_converterReturnCode);
	auto& v_returnCode = m_converterReturnCode.data();

	if (!v_returnCode)
	{
		m_dbProgressStatus->setText("Success!");

		constexpr const static std::string_view v_resultMessages[] =
		{
			"Blueprint has been successfully converted!",
			"Tile has been successfully converted!",
			"World has been successfully converted!",
			"Character has been successfully converted!"
		};

		const std::string_view& v_msg = v_resultMessages[m_converterTypeBox->currentIndex()];
		QtUtil::infoWithSound(
			this,
			"Convert Success",
			QString::fromUtf8(v_msg.data(), v_msg.size())
		);
	}
	else
	{
		m_dbProgressStatus->setText("Fail!");

		QtUtil::errorWithSound(
			this,
			"Convert Error",
			QString::fromStdString(v_returnCode.getErrorMsg())
		);
	}

	v_returnCode.clear();

	this->updateUIState(this->isGameDatabaseLoaded(), this->isUserDatabaseLoaded(), true);
	m_dbProgressBar->setValue(0);
}

void MainGui::databaseLoadedCallback()
{
	m_dbProgressStatus->m_timer->stop();
	m_dbProgressBar->setValue(0);

	if (m_dbLoaderThread)
	{
		delete m_dbLoaderThread;
		m_dbLoaderThread = nullptr;
	}

	const QString v_final_status = QString("Successfully loaded %1 objects from %2 mods")
		.arg(SMMod::GetAmountOfObjects())
		.arg(SMMod::GetAmountOfMods());

	m_dbProgressStatus->setText(v_final_status);

	this->updateUIState(true, this->isUserDatabaseLoaded(), true);
}

void MainGui::updateDbProgressCallback()
{
	if (ProgCounter::State == ProgState::None)
		return;

	std::string v_stateOutput(ProgCounter::GetStateString());
	if (ProgCounter::StateHasNumbers())
	{
		const std::size_t v_max_val = ProgCounter::ProgressMax;
		const std::size_t v_cur_val = std::min(ProgCounter::ProgressValue.load(), v_max_val);

		m_dbProgressBar->setMaximum(int(v_max_val));
		m_dbProgressBar->setValue(int(v_cur_val));

		// Append numbers to the string
		v_stateOutput.append("(");
		v_stateOutput.append(std::to_string(v_cur_val));
		v_stateOutput.append(" / ");
		v_stateOutput.append(std::to_string(v_max_val));
		v_stateOutput.append(")");
	}
	else
	{
		m_dbProgressBar->setValue(m_dbProgressBar->maximum());
	}

	m_dbProgressStatus->setText(QString::fromUtf8(v_stateOutput));
}

void MainGui::converterTypeChanged()
{
	if (this->isUserDatabaseLoaded())
		this->updateSearchResults(0);
	else
		this->updateUserObjectStatusCallback();

	this->updateObjectPathTextBox();
	this->updateSearchTextBox();
	this->updateConvertButton();
}

void MainGui::selectedObjectChanged()
{
	this->updateConvertButton();
	this->updateContextMenu();

	if (m_objectList->selectedIndex() >= 0)
	{
		this->updateObjectPathTextBox();
		m_objectPath->clear();
	}
}

void MainGui::objectPathTextChanged()
{
	this->updateConvertButton();

	if (m_objectPath->text().length() > 0)
		m_objectList->clearSelection();
}

void MainGui::searchTextChanged()
{
	this->updateSearchResults(m_lastSearchLength);
	this->updateObjectPathTextBox();
	this->updateConvertButton();

	m_lastSearchLength = m_searchBox->text().length();
}


std::uint64_t MainGui::getAuthorIdFromSelection()
{
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		return this->getSelectedObjectAuthorId<BlueprintFolderReader>();
	case ConvType_TileConverter:
		return this->getSelectedObjectAuthorId<TileFolderReader>();
	case ConvType_WorldConverter:
		return this->getSelectedObjectAuthorId<WorldFolderReader>();
	default:
		return 0;
	}
}

std::uint64_t MainGui::getWorkshopIdFromSelection()
{
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		return this->getSelectedObjectSteamId<BlueprintFolderReader>();
	case ConvType_TileConverter:
		return this->getSelectedObjectSteamId<TileFolderReader>();
	case ConvType_WorldConverter:
		return this->getSelectedObjectSteamId<WorldFolderReader>();
	default:
		return 0;
	}
}

void MainGui::openDirectory(
	const std::wstring_view& path_view,
	const char* type_str)
{
	const std::wstring v_dir_wide = std::wstring(
		path_view.data(), path_view.size());

	if (!(File::CreateDirectorySafe(v_dir_wide) && QtUtil::openDirInExplorer(v_dir_wide)))
	{
		const QString v_error_str = QString(
			"Failed to show the %1 output directory!").arg(type_str);

		QtUtil::errorWithSound(this, "Error", v_error_str);
	}
}

void MainGui::openProgramSettings()
{
	ProgramSettingsGui v_settings(this);
	v_settings.exec();

	if (v_settings.shouldReloadObjectDatabase())
	{
		DatabaseConfig::ReadConfig();
		GroundTextureDatabase::Initialize();

		this->loadObjectDatabase(false);
	}

	if (v_settings.shouldReloadUserObjects())
		this->loadUserObjects();
}

void MainGui::openFilterSettings()
{
	FilterSettingsGui v_gui(this);
	v_gui.exec();

	if (v_gui.filtersChanged())
		this->updateSearchResults(0);
}

void MainGui::openItemInSteamWorkshop()
{
	QtUtil::openItemInSteam(this->getWorkshopIdFromSelection());
}

void MainGui::openItemInExplorer()
{
	std::wstring v_objPath;
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		v_objPath = this->getSelectedObjectPath<BlueprintFolderReader>();
		break;
	case ConvType_TileConverter:
		v_objPath = this->getSelectedObjectPath<TileFolderReader>();
		break;
	case ConvType_WorldConverter:
		v_objPath = this->getSelectedObjectPath<WorldFolderReader>();
		break;
	default:
		return;
	}

	if (v_objPath.empty())
		return;

	QtUtil::showFileInExplorer(v_objPath);
}

void MainGui::openCreatorProfile()
{
	const std::uint64_t v_itemAuthorId = this->getAuthorIdFromSelection();
	if (!v_itemAuthorId)
		return;

	const QString v_authorUrl = QString("http://steamcommunity.com/profiles/%1")
		.arg(QString::number(v_itemAuthorId, 10));

	QDesktopServices::openUrl(v_authorUrl);
}

void MainGui::copyItemWorkshopId()
{
	const std::uint64_t v_itemWorkshopId = this->getWorkshopIdFromSelection();
	if (!v_itemWorkshopId)
		return;

	QApplication::clipboard()->setText(QString::number(v_itemWorkshopId, 10));
}

void MainGui::copyItemAuthorId()
{
	const std::uint64_t v_itemAuthorId = this->getAuthorIdFromSelection();
	if (!v_itemAuthorId)
		return;

	QApplication::clipboard()->setText(QString::number(v_itemAuthorId, 10));
}

void MainGui::copyItemUuid()
{
	std::string v_itemUuid;
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		v_itemUuid = this->getSelectedObjectUuid<BlueprintFolderReader>();
		break;
	case ConvType_TileConverter:
		v_itemUuid = this->getSelectedObjectUuid<TileFolderReader>();
		break;
	case ConvType_WorldConverter:
		v_itemUuid = this->getSelectedObjectUuid<WorldFolderReader>();
		break;
	default:
		break;
	}

	if (v_itemUuid.empty())
		return;

	QApplication::clipboard()->setText(QString::fromStdString(v_itemUuid));
}

void MainGui::openObjectInfoGui()
{
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		this->openObjectInfoGuiInternal<BlueprintFolderReader>();
		break;
	case ConvType_TileConverter:
		this->openObjectInfoGuiInternal<TileFolderReader>();
		break;
	case ConvType_WorldConverter:
		this->openObjectInfoGuiInternal<WorldFolderReader>();
		break;
	}
}

bool MainGui::convertBlueprint(const std::wstring& filename, const std::wstring& path)
{
	BlueprintConverterSettingsGui v_conv_settings(this, filename, path);
	if (v_conv_settings.exec() != QDialog::DialogCode::Accepted)
		return false;

	BlueprintConverterThreadData v_threadData;
	v_threadData.path = path;
	v_conv_settings.getThreadData(&v_threadData);

	m_converterThread = QThread::create(
		MainGui::ConverterFunction<BlueprintConverterThreadData, BlueprintConv>, this, v_threadData);

	DebugOutL(__FUNCTION__);
	return true;
}

bool MainGui::convertTile(const std::wstring& filename, const std::wstring& path)
{
	TileConverterSettingsGui v_conv_settings(this, filename, path);
	if (v_conv_settings.exec() != QDialog::DialogCode::Accepted)
		return false;

	TileConverterThreadData v_thread_data;
	v_thread_data.path = path;
	v_conv_settings.getThreadData(&v_thread_data);

	m_converterThread = QThread::create(
		MainGui::ConverterFunction<TileConverterThreadData, TileConv>, this, v_thread_data);

	DebugOutL(__FUNCTION__);
	return true;
}

bool MainGui::convertWorld(const std::wstring& filename, const std::wstring& path)
{
	TileConverterSettingsGui v_conv_settings(this, filename, path);
	if (v_conv_settings.exec() != QDialog::DialogCode::Accepted)
		return false;

	TileConverterThreadData v_thread_data;
	v_thread_data.path = path;
	v_conv_settings.getThreadData(&v_thread_data);

	m_converterThread = QThread::create(
		MainGui::ConverterFunction<TileConverterThreadData, WorldConverter>, this, v_thread_data);

	DebugOutL(__FUNCTION__);
	return true;
}

bool MainGui::convertCharacter(const std::wstring& filename, const UserCharacterData& v_data)
{
	DebugOutL(__FUNCTION__);
	return false;
}

bool MainGui::convertSelectedIndex()
{
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
	{
		BlueprintInstance* v_pBlueprint = this->getSelectedObject<BlueprintFolderReader>();
		return this->convertBlueprint(v_pBlueprint->name, v_pBlueprint->path);
	}
	case ConvType_TileConverter:
	{
		TileInstance* v_pTile = this->getSelectedObject<TileFolderReader>();
		return this->convertTile(v_pTile->name, v_pTile->path);
	}
	case ConvType_WorldConverter:
	{
		WorldInstance* v_pWorld = this->getSelectedObject<WorldFolderReader>();
		return this->convertWorld(v_pWorld->name, v_pWorld->path);
	}
	case ConvType_CharacterConverter:
	{
		UserCharacterInstance* v_pChar = this->getSelectedObject<UserCharacterReader>();
		return this->convertCharacter(v_pChar->name, v_pChar->character_data);
	}
	default:
		return false;
	}
}

bool MainGui::convertPath()
{
	namespace fs = std::filesystem;

	const std::wstring v_pathWstr = m_objectPath->text().toStdWString();
	if (!File::Exists(v_pathWstr))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "File doesn't exist!");
		return false;
	}

	if (!File::IsRegularFile(v_pathWstr))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "The path must lead to a file");
		return false;
	}

	const fs::path v_curPath = v_pathWstr;

	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		return this->convertBlueprint(v_curPath.has_stem() ? v_curPath.stem().wstring() : L"UnknownBlueprint", v_pathWstr);
	case ConvType_TileConverter:
		return this->convertTile(v_curPath.has_stem() ? v_curPath.stem().wstring() : L"UnknownTile", v_pathWstr);
	case ConvType_WorldConverter:
		return this->convertWorld(v_curPath.has_stem() ? v_curPath.stem().wstring() : L"UnknownWorld", v_pathWstr);
	default:
		return false;
	}
}

void MainGui::convert()
{
	const bool v_hasSelection = m_objectList->hasSelection();
	if (m_objectPath->text().length() == 0 && !v_hasSelection)
		return;

	const bool v_converterReady = v_hasSelection
		? this->convertSelectedIndex()
		: this->convertPath();

	// The same code for every single converter
	if (v_converterReady)
	{
		this->updateUIState(this->isGameDatabaseLoaded(), this->isUserDatabaseLoaded(), false);
		m_dbProgressStatus->m_timer->start(50);

		QObject::connect(m_converterThread, &QThread::finished, this, &MainGui::converterFinishedCallback);
		m_converterThread->start();
	}
}

void MainGui::updateUIState(bool db_loaded, bool objs_loaded, bool obj_converted)
{
	const bool v_objs_loaded_and_obj_converted = objs_loaded && obj_converted;
	const bool v_db_loaded_and_obj_converted = db_loaded && obj_converted;
	const bool v_everything_loaded = db_loaded && objs_loaded && obj_converted;

	m_objectPathButton->setEnabled(v_db_loaded_and_obj_converted);
	m_objectPath->setEnabled(v_db_loaded_and_obj_converted);
	m_searchFilterButton->setEnabled(v_objs_loaded_and_obj_converted);
	m_searchBox->setEnabled(v_objs_loaded_and_obj_converted);

	m_menuBar->m_reloadObjectDatabaseAction->setEnabled(v_db_loaded_and_obj_converted);
	m_menuBar->m_reloadUserObjectsAction->setEnabled(v_objs_loaded_and_obj_converted);
	m_objectList->setEnabled(v_objs_loaded_and_obj_converted);

	m_convertButton->setEnabled(v_db_loaded_and_obj_converted 
		&& (m_objectPath->text().length() > 0 || m_objectList->selectedIndex() >= 0));
	m_menuBar->m_openProgramSettingsAction->setEnabled(v_everything_loaded);

	m_converterTypeBox->setEnabled(obj_converted);
	m_menuBar->setEnabled(obj_converted);

	m_contextMenu->setEnabled(v_everything_loaded);
}

void MainGui::updateSearchResults(int last_search_length)
{
	const int v_type_idx = m_converterTypeBox->currentIndex();
	this->filterObjectStorage(v_type_idx);

	const QString v_search_qtext = m_searchBox->text();
	const int v_search_length = v_search_qtext.length();

	if (v_search_length > 0)
	{
		const std::wstring v_search_str = v_search_qtext.toLower().toStdWString();

		switch (v_type_idx)
		{
		case ConvType_BlueprintConverter:
			BlueprintFolderReader::Search(v_search_str, last_search_length, v_search_length);
			break;
		case ConvType_TileConverter:
			TileFolderReader::Search(v_search_str, last_search_length, v_search_length);
			break;
		case ConvType_WorldConverter:
			WorldFolderReader::Search(v_search_str, last_search_length, v_search_length);
			break;
		case ConvType_CharacterConverter:
			UserCharacterReader::Search(v_search_str, last_search_length, v_search_length);
			break;
		}
	}

	this->updateObjectList();
}

void MainGui::filterObjectStorage(int conv_type)
{
	switch (conv_type)
	{
	case ConvType_BlueprintConverter:
		BlueprintFolderReader::UpdateStorage();
		break;
	case ConvType_TileConverter:
		TileFolderReader::UpdateStorage();
		break;
	case ConvType_WorldConverter:
		WorldFolderReader::UpdateStorage();
		break;
	case ConvType_CharacterConverter:
		UserCharacterReader::UpdateStorage();
		break;
	}
}

void MainGui::updateUserObjectStatusText(const QString& new_text)
{
	m_objectLoaderStatus->setText(new_text);

	if (m_objectLoaderStatus->isVisible())
	{
		m_objectLoaderStatus->resize(
			m_objectLoaderStatus->fontMetrics().size(0, m_objectLoaderStatus->text()));
		this->updateUserObjectStatusPosition();
	}
}

void MainGui::updateUserObjectStatusPosition()
{
	m_objectLoaderStatus->move(
		m_objectList->x() + (m_objectList->width() - m_objectLoaderStatus->width()) / 2,
		m_objectList->y() + (m_objectList->height() - m_objectLoaderStatus->height()) / 2
	);
}

void MainGui::updateObjectPathTextBox()
{
	constexpr const static std::string_view v_selectedGenMessages[2][4] =
	{
		{
			"Path to Blueprint",
			"Path to Tile",
			"Path to World",
			"Path to Character"
		},
		{
			"Selected a Blueprint in the list",
			"Selected a Tile in the list",
			"Selected a World in the list",
			"Selected a Character in the list"
		}
	};

	const std::string_view& v_curView = v_selectedGenMessages
		[std::size_t(m_objectList->hasSelection())]
		[m_converterTypeBox->currentIndex()];
		
	m_objectPath->setPlaceholderText(
		QString::fromUtf8(v_curView.data(), v_curView.size()));
}

void MainGui::updateSearchTextBox()
{
	constexpr const static std::string_view v_searchTextMsgs[] =
	{
		"Search Blueprints",
		"Search Tiles",
		"Search Worlds",
		"Search Characters"
	};

	const std::string_view& v_curView = v_searchTextMsgs[m_converterTypeBox->currentIndex()];
	m_searchBox->setPlaceholderText(QString::fromUtf8(v_curView.data(), v_curView.size()));
}

void MainGui::updateConvertButton()
{
	m_convertButton->setEnabled((m_objectPath->text().length() > 0 || m_objectList->selectedIndex() >= 0) &&
		this->isGameDatabaseLoaded());
}

void MainGui::updateContextMenu()
{
	const int v_cur_idx = m_converterTypeBox->currentIndex();

	const bool v_is_char_conv = v_cur_idx != ConvType_CharacterConverter;
	m_contextMenu->m_copyUuidAction->setEnabled(v_is_char_conv);
	m_contextMenu->m_showObjectInfoAction->setEnabled(v_is_char_conv);

	const bool v_has_creator_id = this->getAuthorIdFromSelection() != 0;
	m_contextMenu->m_copyCreatorIdAction->setEnabled(v_has_creator_id);
	m_contextMenu->m_openCreatorProfile->setEnabled(v_has_creator_id);

	const bool v_has_workshop_id = this->getWorkshopIdFromSelection() != 0;
	m_contextMenu->m_openInSteamWorkshopAction->setEnabled(v_has_workshop_id);
	m_contextMenu->m_copyWorkshopIdAction->setEnabled(v_has_workshop_id);
}

const char* MainGui::getNoObjectsTextForSpecificConverter() const
{
	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		return "No Blueprints";
	case ConvType_TileConverter:
		return "No Tiles";
	case ConvType_WorldConverter:
		return "No Worlds";
	case ConvType_CharacterConverter:
		return "No Characters";
	default:
		return "No Objects";
	}
}

void MainGui::updateObjectList()
{
	m_objectList->silentClear();

	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		this->storeObjectListInView<BlueprintFolderReader>();
		break;
	case ConvType_TileConverter:
		this->storeObjectListInView<TileFolderReader>();
		break;
	case ConvType_WorldConverter:
		this->storeObjectListInView<WorldFolderReader>();
		break;
	case ConvType_CharacterConverter:
		//this->storeObjectListInView<UserCharacterReader>();
		break;
	}

	m_objectList->updateGraphics();

	const bool v_empty = m_objectList->m_objectStorage.empty();

	m_objectLoaderStatus->setVisible(v_empty);
	if (v_empty)
	{
		this->updateUserObjectStatusText(
			this->getNoObjectsTextForSpecificConverter());
	}
}


void MainGui::loadObjectDatabase(bool reload_key_replacer)
{
	if (m_dbLoaderThread && m_dbLoaderThread->isRunning())
		return;

	m_dbLoaderThread = QThread::create(
		[](bool should_reload) -> void {
			if (should_reload)
				KeywordReplacer::Initialize();

			DatabaseLoader::LoadDatabase();
		},
		reload_key_replacer
	);

	QObject::connect(m_dbLoaderThread, &QThread::finished, this, &MainGui::databaseLoadedCallback);

	this->updateUIState(false, this->isUserDatabaseLoaded(), true);
	m_dbProgressStatus->m_timer->start(50);
	m_dbLoaderThread->start();
}

void MainGui::loadUserObjects()
{
	if (m_userObjectLoaderThread && m_userObjectLoaderThread->isRunning())
		return;

	m_userObjectLoaderThread = QThread::create(UserObjectFolderReader::ReadFromConfig);
	QObject::connect(m_userObjectLoaderThread, &QThread::finished, this, &MainGui::userObjectsLoadedCallback);
	
	m_objectLoaderStatus->setVisible(true);
	m_objectList->clear();
	m_objectList->clearImageCache();

	this->updateUserObjectStatusCallback();
	this->updateObjectPathTextBox();

	this->updateUIState(this->isGameDatabaseLoaded(), false, true);
	m_objectLoaderStatus->m_timer->start(50);
	m_userObjectLoaderThread->start();
}

bool MainGui::isGameDatabaseLoaded()
{
	return m_dbLoaderThread == nullptr;
}

bool MainGui::isUserDatabaseLoaded()
{
	return m_userObjectLoaderThread == nullptr;
}