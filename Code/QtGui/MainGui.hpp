#pragma once

#include "UserObjectListViewWidget.hpp"
#include "Converter/ConvertError.hpp"

#include <QtWidgets/QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QThread>
#include <QLabel>
#include <QTimer>

#include <mutex>

#define MAINGUI_P MainGui::g_mainGuiPtr

enum : unsigned short
{
	ConvType_BlueprintConverter = 0,
	ConvType_TileConverter      = 1,
	ConvType_WorldConverter     = 2,
	ConvType_CharacterConverter = 3
};

class MainGuiMenuBar
{
public:
	MainGuiMenuBar(QWidget* parent = nullptr);
	~MainGuiMenuBar();

	void resize();

	int x() const;
	int y() const;
	int width() const;
	int height() const;

	void setEnabled(bool value);

	QMenuBar* m_menuBar;

	QMenu* m_openMenu;
	QAction* m_openBlueprintOutDirAction;
	QAction* m_openWorldOutDirAction;
	QAction* m_openTileOutDirAction;
	QAction* m_aboutProgramAction;
	QAction* m_aboutQtAction;

	QMenu* m_settingsMenu;
	QAction* m_reloadUserObjectsAction;
	QAction* m_reloadObjectDatabaseAction;
	QAction* m_openProgramSettings;
};

struct UserCharacterData;

class MainGui : public QMainWindow
{
public:
	MainGui(QWidget* parent = nullptr);
	~MainGui();

private:
	void resizeEvent(QResizeEvent* event) override;

	void updateUserObjectStatusCallback();
	void userObjectsLoadedCallback();
	void converterFinishedCallback();
	void databaseLoadedCallback();
	void updateProgressCallback();
	void converterTypeChanged();
	void selectedObjectChanged();
	void objectPathTextChanged();
	void searchTextChanged();

	void openDirectory(const std::wstring_view& path_view, const char* type_str);
	void openProgramSettings();
	void openFilterSettings();

	bool convertBlueprint(const std::wstring& filename, const std::wstring& path);
	bool convertTile(const std::wstring& filename, const std::wstring& path);
	bool convertWorld(const std::wstring& filename, const std::wstring& path);
	bool convertCharacter(const std::wstring& filename, const UserCharacterData& v_data);
	void convertSelectedIndex();
	void convertPath();
	void convert();

	//Object list functions
	template<typename T>
	std::vector<typename T::ObjectType>& getCurrentList()
	{
		return m_searchBox->text().isEmpty() ? T::GetCurrentStorage() : T::SearchResults;
	}

	template<typename T>
	inline void storeObjectListInView()
	{
		for (const typename T::ObjectType v_cur_item : this->getCurrentList<T>())
			m_objectList->addNewObjectSilent(UserObjectItem(
				QString::fromStdWString(v_cur_item->name),
				QString::fromStdString(v_cur_item->uuid.ToString()),
				QString::fromStdWString(v_cur_item->preview_image)));
	}

	template<typename T>
	inline typename T::ObjectType getSelectedObject()
	{
		std::vector<typename T::ObjectType>& v_cur_list = this->getCurrentList<T>();
		const int v_sel_idx = m_objectList->selectedIndex();

		if (v_sel_idx >= 0 && v_sel_idx < v_cur_list.size())
			return v_cur_list[v_sel_idx];

		return nullptr;
	}

	void updateUIState(bool db_loaded, bool objs_loaded, bool obj_converted);
	void updateSearchResults(int last_search_length);
	void filterObjectStorage(int conv_type);
	void updateUserObjectStatusPosition();
	void updateObjectPathTextBox();
	void updateSearchTextBox();
	void updateConvertButton();
	void updateObjectList();

	void loadObjectDatabase(bool reload_key_replacer);
	void loadUserObjects();

	bool isGameDatabaseLoaded();
	bool isUserDatabaseLoaded();

public:
	inline static MainGui* g_mainGuiPtr = nullptr;

private:
	int m_lastSearchLength = 0;

	MainGuiMenuBar m_menuBar;
	
	UserObjectListView* m_objectList;
	QTimer* m_objectLoaderStatusUpdater;
	QLabel* m_objectLoaderStatus;

	QLineEdit* m_objectPath;
	QLineEdit* m_searchBox;

	QPushButton* m_selectPathButton;
	QPushButton* m_filterButton;

	QPushButton* m_convertButton;
	QComboBox* m_converterTypeBox;

	QProgressBar* m_progressBar;
	QLabel* m_progressStatus;
	QTimer* m_progressUpdater;

	QThread* m_dbLoaderThread;
	QThread* m_userObjectLoaderThread;

	std::mutex m_converterReturnCodeMutex;
	ConvertError m_converterReturnCode;
	QThread* m_converterThread;
};