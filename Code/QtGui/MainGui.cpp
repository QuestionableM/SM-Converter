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
#include "Converter/TileConverter/TileConverter.hpp"
#include "Converter/ConvertSettings.hpp"

#include "BlueprintConvertSettingsGui.hpp"
#include "TileConverterSettingsGui.hpp"
#include "ProgramSettingsGui.hpp"
#include "FilterSettingsGui.hpp"
#include "AboutProgramGui.hpp"

#include "Utils/File.hpp"
#include "QtUtil.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QMenuBar>

MainGuiMenuBar::MainGuiMenuBar(QWidget* parent)
{
	m_menuBar = new QMenuBar(parent);
	m_menuBar->setObjectName("main_menu_bar");

	m_openMenu = new QMenu(m_menuBar);
	m_openMenu->setObjectName("open_menu");
	m_openMenu->setTitle("Open");

	m_openBlueprintOutDirAction = new QAction(parent);
	m_openBlueprintOutDirAction->setObjectName("a_open_bp_out_dir");
	m_openBlueprintOutDirAction->setText("Blueprint output directory");
	m_openMenu->addAction(m_openBlueprintOutDirAction);

	m_openWorldOutDirAction = new QAction(parent);
	m_openWorldOutDirAction->setObjectName("a_open_wlrd_dir");
	m_openWorldOutDirAction->setText("World Output Directory");
	m_openMenu->addAction(m_openWorldOutDirAction);

	m_openTileOutDirAction = new QAction(parent);
	m_openTileOutDirAction->setObjectName("a_open_tile_dir");
	m_openTileOutDirAction->setText("Tile Output Directory");
	m_openMenu->addAction(m_openTileOutDirAction);

	m_openMenu->addSeparator();

	m_aboutProgramAction = new QAction(parent);
	m_aboutProgramAction->setObjectName("a_about_program");
	m_aboutProgramAction->setText("About Program");
	m_openMenu->addAction(m_aboutProgramAction);

	m_openMenu->addSeparator();

	m_aboutQtAction = new QAction(parent);
	m_aboutQtAction->setObjectName("a_about_qt");
	m_aboutQtAction->setText("About QT");
	QObject::connect(m_aboutQtAction, &QAction::triggered, QApplication::aboutQt);

	m_openMenu->addAction(m_aboutQtAction);

	m_settingsMenu = new QMenu(m_menuBar);
	m_settingsMenu->setObjectName("settings_menu");
	m_settingsMenu->setTitle("Settings");

	m_reloadUserObjectsAction = new QAction(parent);
	m_reloadUserObjectsAction->setObjectName("a_reload_user_objs");
	m_reloadUserObjectsAction->setText("Reload User Objects");
	m_settingsMenu->addAction(m_reloadUserObjectsAction);

	m_reloadObjectDatabaseAction = new QAction(parent);
	m_reloadObjectDatabaseAction->setObjectName("a_reload_obj_db");
	m_reloadObjectDatabaseAction->setText("Reload Object Database");
	m_settingsMenu->addAction(m_reloadObjectDatabaseAction);

	m_settingsMenu->addSeparator();

	m_openProgramSettings = new QAction(parent);
	m_openProgramSettings->setObjectName("a_open_psettings");
	m_openProgramSettings->setText("Settings");
	m_settingsMenu->addAction(m_openProgramSettings);

	m_menuBar->addAction(m_openMenu->menuAction());
	m_menuBar->addAction(m_settingsMenu->menuAction());
}


MainGuiMenuBar::~MainGuiMenuBar()
{
	delete m_menuBar;
}

void MainGuiMenuBar::resize()
{
	m_menuBar->setGeometry(0, 0, m_menuBar->parentWidget()->width(), 22);
}

int MainGuiMenuBar::x() const { return m_menuBar->x(); }
int MainGuiMenuBar::y() const { return m_menuBar->y(); }
int MainGuiMenuBar::width() const { return m_menuBar->width(); }
int MainGuiMenuBar::height() const { return m_menuBar->height(); }

void MainGuiMenuBar::setEnabled(bool value)
{
	m_menuBar->setEnabled(value);
}

MainGui::MainGui(QWidget* parent)
	: QMainWindow(parent),
	m_menuBar(this)
{
	g_mainGuiPtr = this;

	//UI INITIALIZATION
	this->setWindowTitle(
		"SM Converter"
#if defined(_DEBUG) || defined(DEBUG)
		" (DEBUG)"
#endif
	);
	this->setMinimumSize(450, 350);
	this->resize(450, 350);

	m_objectList = new UserObjectListView(this);
	m_objectList->setObjectName("obj_list");
	m_objectList->setEnabled(false);
	QObject::connect(m_objectList, &UserObjectListView::selectedIndexChanged, this, &MainGui::selectedObjectChanged);

	m_objectLoaderStatusUpdater = new QTimer(this);
	m_objectLoaderStatusUpdater->setObjectName("tmr_usr_obj_status_updater");
	QObject::connect(m_objectLoaderStatusUpdater, &QTimer::timeout, this, &MainGui::updateUserObjectStatusCallback);

	m_objectLoaderStatus = new QLabel("NONE", this);
	m_objectLoaderStatus->setObjectName("lbl_usr_obj_status");

	//Add button callbacks
	QObject::connect(m_menuBar.m_reloadUserObjectsAction, &QAction::triggered, this, &MainGui::loadUserObjects);
	QObject::connect(m_menuBar.m_reloadObjectDatabaseAction, &QAction::triggered, this,
		[this]() -> void { loadObjectDatabase(false); });

	QObject::connect(m_menuBar.m_openBlueprintOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::BlueprintOutputFolder, "blueprint"); });
	QObject::connect(m_menuBar.m_openWorldOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::WorldOutputFolder, "world"); });
	QObject::connect(m_menuBar.m_openTileOutDirAction, &QAction::triggered, this,
		[this]() -> void { openDirectory(DatabaseConfig::TileOutputFolder, "tile"); });
	QObject::connect(m_menuBar.m_aboutProgramAction, &QAction::triggered, this,
		[this]() -> void { AboutProgramGui(this).exec(); });
	QObject::connect(
		m_menuBar.m_openProgramSettings, &QAction::triggered,
		this, &MainGui::openProgramSettings);

	m_objectPath = new QLineEdit(this);
	m_objectPath->setObjectName("le_object_path");
	m_objectPath->setPlaceholderText("Object path");
	QObject::connect(m_objectPath, &QLineEdit::textChanged, this, &MainGui::objectPathTextChanged);

	m_searchBox = new QLineEdit(this);
	m_searchBox->setObjectName("le_search_box");
	m_searchBox->setPlaceholderText("Search");
	QObject::connect(m_searchBox, &QLineEdit::textChanged, this, &MainGui::searchTextChanged);

	m_selectPathButton = new QPushButton(this);
	m_selectPathButton->setObjectName("pbtn_path");
	m_selectPathButton->setText("...");
	QObject::connect(m_selectPathButton, &QPushButton::pressed, this,
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

	m_filterButton = new QPushButton(this);
	m_filterButton->setObjectName("pbtn_filter");
	m_filterButton->setText("Filter");
	QObject::connect(m_filterButton, &QPushButton::clicked, this, &MainGui::openFilterSettings);

	m_progressBar = new QProgressBar(this);
	m_progressBar->setObjectName("pb_progress");
	m_progressBar->setTextVisible(false);

	m_progressStatus = new QLabel(this);
	m_progressStatus->setObjectName("lbl_progress_status");

	m_convertButton = new QPushButton(this);
	m_convertButton->setObjectName("pbtn_convert");
	m_convertButton->setText("Convert");
	m_convertButton->setEnabled(false);
	QObject::connect(m_convertButton, &QPushButton::clicked, this, &MainGui::convert);

	m_converterTypeBox = new QComboBox(this);
	m_converterTypeBox->setObjectName("cb_converter_type");

	m_converterTypeBox->addItem("Blueprint Converter (.json, .blueprint)");
	m_converterTypeBox->addItem("Tile Converter (.tile)");

#if defined(SMCONV_ENABLE_WORLD_CONVERTER) //Mostly finished
	m_converterTypeBox->addItem("World Converter (.json, .world)");
#endif

#if defined(SMCONV_ENABLE_CHARACTER_CONVERTER) //WIP
	m_converterTypeBox->addItem("Character Converter");
#endif

	m_converterTypeBox->setCurrentIndex(0);
	QObject::connect(m_converterTypeBox, &QComboBox::currentIndexChanged, this, &MainGui::converterTypeChanged);

	m_progressUpdater = new QTimer(this);
	m_progressUpdater->setObjectName("tmr_status_updater");
	QObject::connect(m_progressUpdater, &QTimer::timeout, this, &MainGui::updateProgressCallback);

	//DATABASE INITIALIZATION
	DatabaseLoader::InitializeDatabase();

	m_dbLoaderThread = nullptr;
	this->loadObjectDatabase(true);

	m_userObjectLoaderThread = nullptr;
	this->loadUserObjects();

	this->updateSearchTextBox();
}

MainGui::~MainGui()
{
	g_mainGuiPtr = nullptr;
}

constexpr int g_windowMargin = 10;
constexpr int g_convertBtnWidth = 100;
constexpr int g_utilBtnWidth = 75;
constexpr int g_commonWidgetHeight = 22;
constexpr int g_progressBarHeight = 15;

void MainGui::resizeEvent(QResizeEvent* event)
{
	this->blockSignals(true);

	m_menuBar.resize();

	const int v_util_btn_start_x = this->width() - g_windowMargin - g_utilBtnWidth;
	const int v_edit_box_width = this->width() - g_utilBtnWidth - g_windowMargin * 3;
	const int v_convert_btn_height = m_convertButton->fontMetrics().height() + g_windowMargin * 2;

	m_selectPathButton->setGeometry(
		v_util_btn_start_x,
		m_menuBar.y() + m_menuBar.height() + g_windowMargin,
		g_utilBtnWidth,
		g_commonWidgetHeight);

	m_objectPath->setGeometry(
		g_windowMargin,
		m_selectPathButton->y(),
		v_edit_box_width,
		g_commonWidgetHeight);

	m_filterButton->setGeometry(
		v_util_btn_start_x,
		m_selectPathButton->y() + m_selectPathButton->height() + g_windowMargin,
		g_utilBtnWidth,
		g_commonWidgetHeight);

	m_searchBox->setGeometry(
		g_windowMargin,
		m_filterButton->y(),
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

	m_progressBar->setGeometry(
		g_windowMargin,
		this->height() - g_progressBarHeight - g_windowMargin,
		this->width() - m_convertButton->width() - g_windowMargin * 3,
		g_progressBarHeight);

	m_progressStatus->setGeometry(
		g_windowMargin,
		m_convertButton->y(),
		m_progressBar->width(),
		m_progressStatus->fontMetrics().height());

	const int v_object_list_y = m_searchBox->y() + m_searchBox->height() + g_windowMargin;

	m_objectList->setGeometry(
		g_windowMargin,
		v_object_list_y,
		this->width() - g_windowMargin * 2,
		(m_converterTypeBox->y() - g_windowMargin) - v_object_list_y);

	this->updateUserObjectStatusPosition();

	this->blockSignals(false);
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
			.arg(BlueprintFolderReader::ObjectCounter);
		break;
	case ConvType_TileConverter:
		v_status_text = QString("Loading Tiles... (%1)")
			.arg(TileFolderReader::ObjectCounter);
		break;
	case ConvType_WorldConverter:
		v_status_text = QString("Loading Worlds... (%1)")
			.arg(WorldFolderReader::ObjectCounter);
		break;
	case ConvType_CharacterConverter:
		v_status_text = QString("Loading Characters... (%1)")
			.arg(UserCharacterReader::ObjectCounter);
		break;
	}

	m_objectLoaderStatus->setText(v_status_text);
	m_objectLoaderStatus->resize(
		m_objectLoaderStatus->fontMetrics().size(0, m_objectLoaderStatus->text()));
	this->updateUserObjectStatusPosition();
}

void MainGui::userObjectsLoadedCallback()
{
	m_objectLoaderStatusUpdater->stop();
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
	m_progressUpdater->stop();
	m_progressBar->setValue(m_progressBar->maximum());

	if (m_converterThread)
	{
		delete m_converterThread;
		m_converterThread = nullptr;
	}

	std::lock_guard v_lock_g(m_converterReturnCodeMutex);

	if (!m_converterReturnCode)
	{
		m_progressStatus->setText("Success!");

		constexpr const static std::wstring_view v_resultMessages[] =
		{
			L"Blueprint has been successfully converted!",
			L"Tile has been successfully converted!",
			L"World has been successfully converted!",
			L"Character has been successfully converted!"
		};

		const std::wstring_view& v_msg = v_resultMessages[m_converterTypeBox->currentIndex()];
		QtUtil::infoWithSound(this,
			"Convert Success",
			QString::fromWCharArray(v_msg.data(), v_msg.size()));
	}
	else
	{
		m_progressStatus->setText("Fail!");

		QtUtil::errorWithSound(this,
			"Convert Error",
			QString::fromStdWString(m_converterReturnCode.GetErrorMsg()));
	}

	m_converterReturnCode.clear();

	this->updateUIState(this->isGameDatabaseLoaded(), this->isUserDatabaseLoaded(), true);
	m_progressBar->setValue(0);
}

void MainGui::databaseLoadedCallback()
{
	m_progressUpdater->stop();
	m_progressBar->setValue(0);

	if (m_dbLoaderThread)
	{
		delete m_dbLoaderThread;
		m_dbLoaderThread = nullptr;
	}

	const std::size_t v_mod_count = SMMod::GetAmountOfMods();
	const std::size_t v_obj_count = SMMod::GetAmountOfObjects();

	m_progressStatus->setText(
		QString("Successfully loaded %1 objects from %2 mods").arg(v_obj_count).arg(v_mod_count));

	this->updateUIState(true, this->isUserDatabaseLoaded(), true);
}

void MainGui::updateProgressCallback()
{
	if (ProgCounter::State == ProgState::None)
		return;

	std::wstring v_state_output = ProgCounter::GetStateString();
	if (ProgCounter::StateHasNumbers())
	{
		const std::size_t v_max_val = ProgCounter::ProgressMax;
		const std::size_t v_cur_val = std::min(ProgCounter::ProgressValue.load(), v_max_val);

		m_progressBar->setMaximum(int(v_max_val));
		m_progressBar->setValue(int(v_cur_val));

		//Append numbers to the string
		v_state_output.append(L"(");
		v_state_output.append(std::to_wstring(v_cur_val));
		v_state_output.append(L" / ");
		v_state_output.append(std::to_wstring(v_max_val));
		v_state_output.append(L")");
	}
	else
	{
		m_progressBar->setValue(m_progressBar->maximum());
	}

	m_progressStatus->setText(QString::fromStdWString(v_state_output));
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
	//Update the convert button here
	this->updateConvertButton();
	//Update context menu strip here 

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

bool MainGui::convertBlueprint(const std::wstring& filename, const std::wstring& path)
{
	BlueprintConverterSettingsGui v_conv_settings(this, filename, path);
	if (v_conv_settings.exec() != QDialog::DialogCode::Accepted)
		return false;

	BlueprintConverterThreadData v_thread_data;
	v_thread_data.path = path;
	v_conv_settings.getThreadData(&v_thread_data);

	m_converterThread = QThread::create(
		[this](BlueprintConverterThreadData thread_data) -> void
		{
			std::lock_guard v_lock_g(m_converterReturnCodeMutex);

			thread_data.applySettings();

			BlueprintConv::ConvertToModel(
				thread_data.path,
				thread_data.name,
				m_converterReturnCode,
				thread_data.custom_game_idx.getPtr());
		},
		v_thread_data
	);

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
		[this](TileConverterThreadData thread_data) -> void
		{
			std::lock_guard v_lock_g(m_converterReturnCodeMutex);

			thread_data.applySettings();
			
			TileConv::ConvertToModel(
				thread_data.path,
				thread_data.name,
				m_converterReturnCode,
				thread_data.custom_game_idx.getPtr());
		},
		v_thread_data
	);

	DebugOutL(__FUNCTION__);
	return true;
}

bool MainGui::convertWorld(const std::wstring& filename, const std::wstring& path)
{
	DebugOutL(__FUNCTION__);
	return false;
}

bool MainGui::convertCharacter(const std::wstring& filename, const UserCharacterData& v_data)
{
	DebugOutL(__FUNCTION__);
	return false;
}

void MainGui::convertSelectedIndex()
{
	bool v_converter_ready = false;

	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
		{
			BlueprintInstance* v_cur_bp = this->getSelectedObject<BlueprintFolderReader>();
			v_converter_ready = this->convertBlueprint(v_cur_bp->name, v_cur_bp->path);
			break;
		}
	case ConvType_TileConverter:
		{
			TileInstance* v_cur_tile = this->getSelectedObject<TileFolderReader>();
			v_converter_ready = this->convertTile(v_cur_tile->name, v_cur_tile->path);
			break;
		}
	case ConvType_WorldConverter:
		{
			WorldInstance* v_cur_world = this->getSelectedObject<WorldFolderReader>();
			v_converter_ready = this->convertWorld(v_cur_world->name, v_cur_world->path);
			break;
		}
	case ConvType_CharacterConverter:
		{
			UserCharacterInstance* v_cur_char = this->getSelectedObject<UserCharacterReader>();
			v_converter_ready = this->convertCharacter(v_cur_char->name, v_cur_char->character_data);
			break;
		}
	}

	//The same code for every single converter
	if (v_converter_ready)
	{
		this->updateUIState(this->isGameDatabaseLoaded(), this->isUserDatabaseLoaded(), false);
		m_progressUpdater->start(50);

		QObject::connect(m_converterThread, &QThread::finished, this, &MainGui::converterFinishedCallback);
		m_converterThread->start();
	}
}

void MainGui::convertPath()
{
	namespace fs = std::filesystem;

	const std::wstring v_path_wstr = m_objectPath->text().toStdWString();
	if (!File::Exists(v_path_wstr))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "File doesn't exist!");
		return;
	}

	if (!File::IsRegularFile(v_path_wstr))
	{
		QtUtil::warningWithSound(this, "Invalid Path", "The path must lead to a file");
		return;
	}

	const fs::path v_cur_path = v_path_wstr;

	switch (m_converterTypeBox->currentIndex())
	{
	case ConvType_BlueprintConverter:
	{
		const std::wstring v_bp_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownBlueprint");
		this->convertBlueprint(v_bp_name, v_path_wstr);
		break;
	}
	case ConvType_TileConverter:
	{
		const std::wstring v_tile_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownTile");
		this->convertTile(v_tile_name, v_path_wstr);
		break;
	}
	case ConvType_WorldConverter:
	{
		const std::wstring v_world_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownWorld");
		this->convertWorld(v_world_name, v_path_wstr);
		break;
	}
	case ConvType_CharacterConverter:
		//TODO: Implement that
		break;
	}
}

void MainGui::convert()
{
	const bool v_has_selection = m_objectList->hasSelection();
	if (m_objectPath->text().length() == 0 && !v_has_selection)
		return;

	if (v_has_selection)
		this->convertSelectedIndex();
	else
		this->convertPath();
}

void MainGui::updateUIState(bool db_loaded, bool objs_loaded, bool obj_converted)
{
	const bool v_objs_loaded_and_obj_converted = objs_loaded && obj_converted;
	const bool v_db_loaded_and_obj_converted = db_loaded && obj_converted;

	m_selectPathButton->setEnabled(v_db_loaded_and_obj_converted);
	m_filterButton->setEnabled(v_db_loaded_and_obj_converted);
	m_objectPath->setEnabled(v_db_loaded_and_obj_converted);
	m_searchBox->setEnabled(v_db_loaded_and_obj_converted);

	m_menuBar.m_reloadObjectDatabaseAction->setEnabled(v_db_loaded_and_obj_converted);
	m_menuBar.m_reloadUserObjectsAction->setEnabled(v_objs_loaded_and_obj_converted);
	m_objectList->setEnabled(v_objs_loaded_and_obj_converted);

	m_convertButton->setEnabled(v_db_loaded_and_obj_converted 
		&& (m_objectPath->text().length() > 0 || m_objectList->selectedIndex() >= 0));
	m_menuBar.m_openProgramSettings->setEnabled(db_loaded && objs_loaded && obj_converted);

	m_converterTypeBox->setEnabled(obj_converted);
	m_menuBar.setEnabled(obj_converted);

	//Update context menu strips here
	/*
		m_cms_blueprint->Enabled = objlist_and_obj_loaded;
		m_btn_openBlueprintFolder->Enabled = false;
		m_btn_openBlueprintInSteamWorkshop->Enabled = false;

		m_cms_tile->Enabled = objlist_and_obj_loaded;
		m_btn_openTileInSteamWorkshop->Enabled = false;
		m_btn_openTileFolder->Enabled = false;
		m_btn_findTileCreatorInSteam->Enabled = false;

		if (objlist_and_obj_loaded)
			this->UpdateContextMenuStrip();
	*/
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

void MainGui::updateUserObjectStatusPosition()
{
	m_objectLoaderStatus->move(
		m_objectList->x() + (m_objectList->width() - m_objectLoaderStatus->width()) / 2,
		m_objectList->y() + (m_objectList->height() - m_objectLoaderStatus->height()) / 2
	);
}

void MainGui::updateObjectPathTextBox()
{
	constexpr const static std::wstring_view v_selectedGenMessages[2][4] =
	{
		{
			L"Path to Blueprint",
			L"Path to Tile",
			L"Path to World",
			L"Path to Character"
		},
		{
			L"Selected a Blueprint in the list",
			L"Selected a Tile in the list",
			L"Selected a World in the list",
			L"Selected a Character in the list"
		}
	};

	const std::wstring_view& v_cur_view = v_selectedGenMessages
		[std::size_t(m_objectList->hasSelection())]
		[m_converterTypeBox->currentIndex()];
		
	m_objectPath->setPlaceholderText(
		QString::fromWCharArray(v_cur_view.data(), v_cur_view.size()));
}

void MainGui::updateSearchTextBox()
{
	constexpr const static std::wstring_view v_searchTextMsgs[] =
	{
		L"Search Blueprints",
		L"Search Tiles",
		L"Search Worlds",
		L"Search Characters"
	};

	const std::wstring_view& v_cur_view = v_searchTextMsgs[m_converterTypeBox->currentIndex()];

	m_searchBox->setPlaceholderText(
		QString::fromWCharArray(v_cur_view.data(), v_cur_view.size()));
}

void MainGui::updateConvertButton()
{
	m_convertButton->setEnabled((m_objectPath->text().length() > 0 || m_objectList->selectedIndex() >= 0) &&
		this->isGameDatabaseLoaded());
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
	m_progressUpdater->start(50);
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
	m_objectLoaderStatusUpdater->start(50);
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