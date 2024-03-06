#pragma once

#include "CustomWidgets/UserObjectListViewWidget.hpp"
#include "CustomWidgets/LabelTimer.hpp"
#include "MainGuiContextMenu.hpp"
#include "MainGuiMenuBar.hpp"

#include "Converter/ConvertError.hpp"
#include "Utils/MutexData.hpp"

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

struct UserCharacterData;

template<typename T, typename = int>
struct HasCreatorId : std::false_type {};

template<typename T>
struct HasCreatorId<T, decltype((void)T::creator_id, 0)> : std::true_type {};

class MainGui : public QMainWindow
{
public:
	MainGui(QWidget* parent = nullptr);
	~MainGui();

	void initializeUI();
	void connectEvents();
	void initializeDatabase();

private:
	void resizeEvent(QResizeEvent* event) override;

	void updateUserObjectStatusCallback();
	void userObjectsLoadedCallback();
	void converterFinishedCallback();
	void databaseLoadedCallback();
	void updateDbProgressCallback();
	void converterTypeChanged();
	void selectedObjectChanged();
	void objectPathTextChanged();
	void searchTextChanged();

	std::uint64_t getAuthorIdFromSelection();
	std::uint64_t getWorkshopIdFromSelection();

	void openDirectory(const std::wstring_view& path_view, const char* type_str);
	void openProgramSettings();
	void openFilterSettings();

	//Context menu functions
	void openItemInSteamWorkshop();
	void openItemInExplorer();
	void openCreatorProfile();
	void copyItemWorkshopId();
	void copyItemAuthorId();
	void copyItemUuid();

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

	template<typename T>
	inline std::uint64_t getSelectedObjectSteamId()
	{
		typename T::ObjectType v_curObj = this->getSelectedObject<T>();
		if (v_curObj)
			return v_curObj->workshop_id;

		return 0;
	}

	template<typename T>
	inline std::wstring getSelectedObjectPath()
	{
		typename T::ObjectType v_curObj = this->getSelectedObject<T>();
		if (v_curObj)
			return v_curObj->path;

		return L"";
	}

	template<typename T>
	inline std::uint64_t getSelectedObjectAuthorId()
	{
		if constexpr (
			HasCreatorId<std::remove_pointer_t<typename T::ObjectType>>::value)
		{
			typename T::ObjectType v_curObj = this->getSelectedObject<T>();
			if (v_curObj)
				return v_curObj->creator_id;
		}

		return 0;
	}

	template<typename T>
	inline std::string getSelectedObjectUuid()
	{
		typename T::ObjectType v_curObj = this->getSelectedObject<T>();
		if (v_curObj)
			return v_curObj->uuid.ToString();

		return "";
	}

	void updateUIState(bool db_loaded, bool objs_loaded, bool obj_converted);
	void updateSearchResults(int last_search_length);
	void filterObjectStorage(int conv_type);
	void updateUserObjectStatusPosition();
	void updateObjectPathTextBox();
	void updateSearchTextBox();
	void updateConvertButton();
	void updateContextMenu();
	void updateObjectList();

	void loadObjectDatabase(bool reload_key_replacer);
	void loadUserObjects();

	bool isGameDatabaseLoaded();
	bool isUserDatabaseLoaded();

public:
	inline static MainGui* g_mainGuiPtr = nullptr;

private:
	int m_lastSearchLength = 0;

	ObjectListContextMenu* m_contextMenu;
	MainGuiMenuBar* m_menuBar;
	
	QPushButton* m_objectPathButton;
	QLineEdit* m_objectPath;
	QPushButton* m_searchFilterButton;
	QLineEdit* m_searchBox;

	QProgressBar* m_dbProgressBar;
	LabelTimer* m_dbProgressStatus;
	QThread* m_dbLoaderThread;

	UserObjectListView* m_objectList;
	LabelTimer* m_objectLoaderStatus;
	QThread* m_userObjectLoaderThread;

	QComboBox* m_converterTypeBox;
	QPushButton* m_convertButton;
	QThread* m_converterThread;

	MutexData<ConvertError> m_converterReturnCode;
};