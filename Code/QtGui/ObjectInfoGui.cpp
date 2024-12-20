#include "ObjectInfoGui.hpp"

#include "ObjectDatabase/UserDataReaders/BlueprintFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/WorldFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/TileFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/CustomGameSwitch.hpp"

#include "Converter/WorldConverter/World.hpp"
#include "Converter/Entity/Blueprint.hpp"

#include "Utils/Console.hpp"
#include "QtGui/QtUtil.hpp"

#include <QContextMenuEvent>

ModListContextMenu::ModListContextMenu(QWidget* parent) :
	QMenu(parent),
	m_openInSteamWorkshopAction(new QAction("Open in Steam Workshop", this)),
	m_openInExplorerAction(new QAction("Open in File Explorer", this))
{
	this->addAction(m_openInSteamWorkshopAction);
	this->addAction(m_openInExplorerAction);
}

////////////////////////MOD LIST WIDGET////////////////////////

ModListWidget::ModListWidget(QWidget* parent) :
	QListWidget(parent),
	m_noModsLabel(new CenteredLabel("No Mods", this)),
	m_contextMenu(new ModListContextMenu(this))
{
	this->setSelectionMode(QAbstractItemView::SingleSelection);

	QObject::connect(
		m_contextMenu->m_openInSteamWorkshopAction, &QAction::triggered,
		this, &ModListWidget::openModInWorkshop);

	QObject::connect(
		m_contextMenu->m_openInExplorerAction, &QAction::triggered,
		this, &ModListWidget::openModInExplorer);
}

SMMod* ModListWidget::getSelectedMod() const
{
	const int v_curIdx = this->currentIndex().row();
	if (v_curIdx < 0 || v_curIdx > int(ItemModStats::ModVector.size()))
		return nullptr;

	return ItemModStats::ModVector[v_curIdx]->m_pMod;
}

void ModListWidget::updateContextMenu()
{
	SMMod* v_cur_mod = this->getSelectedMod();
	if (!v_cur_mod)
	{
		m_contextMenu->m_openInSteamWorkshopAction->setEnabled(false);
		m_contextMenu->m_openInExplorerAction->setEnabled(false);
		return;
	}

	m_contextMenu->m_openInSteamWorkshopAction->setEnabled(v_cur_mod->GetWorkshopId() != 0);
	m_contextMenu->m_openInExplorerAction->setEnabled(File::Exists(v_cur_mod->GetDirectory()));
}

void ModListWidget::updateModList()
{
	m_noModsLabel->setVisible(ItemModStats::ModVector.empty());

	this->blockSignals(true);
	this->clear();

	std::wstring v_modName;

	for (const ItemModInstance* v_curMod : ItemModStats::ModVector)
	{
		const std::wstring_view v_modNameView = (v_curMod->m_pMod != nullptr)
			? std::wstring_view(v_curMod->m_pMod->GetName())
			: L"UNKNOWN_MOD";

		v_modName.assign(v_modNameView);
		v_modName.append(L" (");
		v_modName.append(std::to_wstring(v_curMod->m_partCount));
		v_modName.append(L")");

		this->addItem(QString::fromStdWString(v_modName));
	}

	this->blockSignals(false);
	this->update();
}

void ModListWidget::openModInWorkshop()
{
	SMMod* v_cur_mod = this->getSelectedMod();
	if (v_cur_mod)
		QtUtil::openItemInSteam(v_cur_mod->GetWorkshopId());
}

void ModListWidget::openModInExplorer()
{
	SMMod* v_cur_mod = this->getSelectedMod();
	if (!(v_cur_mod && File::Exists(v_cur_mod->GetDirectory())))
	{
		QtUtil::warningWithSound(this, "Invalid Mod", "Mod Folder doesn't exist!");
		return;
	}

	QtUtil::openDirInExplorer(v_cur_mod->GetDirectory());
}

void ModListWidget::contextMenuEvent(QContextMenuEvent* event)
{
	this->updateContextMenu();
	m_contextMenu->exec(event->globalPos());
}

/////////////////////////OBJECT INFO GUI//////////////////////////

ObjectInfoGui::ObjectInfoGui(const QString& title, const std::wstring& image, QWidget* parent) :
	QDialog(parent),
	m_mainLayout(new QVBoxLayout(this)),
	m_mainInfoLayout(new QHBoxLayout(this)),
	m_objectImage(new ImageBox(QString::fromStdWString(image))),
	m_infoLayout(new QVBoxLayout(this)),
	m_modList(new ModListWidget(this))
{
	this->setWindowTitle(title);

	m_objectImage->setFixedSize(100, 100);
	m_mainLayout->addLayout(m_mainInfoLayout);
	m_mainLayout->addWidget(new QLabel("Mod List"), 0, Qt::AlignLeft | Qt::AlignBottom);
	m_mainLayout->addWidget(m_modList);

	m_mainInfoLayout->setAlignment(Qt::AlignTop);
	m_mainInfoLayout->addWidget(m_objectImage);
	m_mainInfoLayout->addLayout(m_infoLayout);

	m_infoLayout->setAlignment(Qt::AlignTop);
	m_infoLayout->setSpacing(1);
	const QMargins v_old_margins = m_infoLayout->contentsMargins();
	m_infoLayout->setContentsMargins(v_old_margins.left(), 0, v_old_margins.right(), 0);

	ItemModStats::Reset();
}

ObjectInfoGui::ObjectInfoGui(BlueprintInstance* bprint, QWidget* parent)
	: ObjectInfoGui("Blueprint Info", bprint->preview_image, parent)
{
	SMBlueprint::CountFromFile(bprint->path);
	
	this->appendMainObjectInfo<BlueprintInstance>(bprint);
	this->appendPartsStats();
	m_modList->updateModList();
}

static bool readWorldData(
	const std::wstring& world_path,
	std::size_t& out_world_width,
	ConvertError& v_error)
{
	TileFolderReader::InitializeTileKeys();

	SMWorld* v_world = nullptr;
	CustomGame* v_cur_cg = SMMod::GetCustomGameFromPath(world_path);
	if (v_cur_cg)
	{
		SMModCustomGameSwitch<false, true> v_cg_switch;
		v_cg_switch.MergeContent(v_cur_cg);

		v_world = SMWorld::LoadFromFile<true>(world_path, v_error);
	}
	else
	{
		v_world = SMWorld::LoadFromFile<true>(world_path, v_error);
	}

	if (!v_world)
		return false;

	out_world_width = v_world->GetWidth();
	delete v_world;
	return true;
}

ObjectInfoGui::ObjectInfoGui(WorldInstance* world, QWidget* parent) :
	ObjectInfoGui("World Info", world->preview_image, parent)
{
	ConvertError v_error;
	std::size_t v_world_width;
	if (!readWorldData(world->path, v_world_width, v_error))
	{
		QtUtil::errorWithSound(this, "World Read Error", "Unknown Error");
		return;
	}

	if (v_error)
	{
		const QString v_formattedError = QString("Error: %1")
			.arg(QString::fromStdString(v_error.getErrorMsg()));

		QtUtil::errorWithSound(this, "World Read Error", v_formattedError);
		return;
	}

	this->appendMainObjectInfo<WorldInstance>(world);
	m_infoLayout->addWidget(
		new QLabel(QString("World Size: %1x%1").arg(v_world_width), this));
	
	this->appendPartsStats();
	m_modList->updateModList();
}

static void readTileData(const std::wstring& tile_path, ConvertError& v_error)
{
	CustomGame* v_cur_cg = SMMod::GetCustomGameFromPath(tile_path);
	if (v_cur_cg)
	{
		SMModCustomGameSwitch<false, true> v_cg_switch;
		v_cg_switch.MergeContent(v_cur_cg);

		TileFolderReader::GetTileData(tile_path, v_error);
	}
	else
	{
		TileFolderReader::GetTileData(tile_path, v_error);
	}
}

ObjectInfoGui::ObjectInfoGui(TileInstance* tile, QWidget* parent)
	: ObjectInfoGui("Tile Info", tile->preview_image, parent)
{
	ConvertError v_error;
	readTileData(tile->path, v_error);

	if (v_error)
	{
		const QString v_error_str = QString("Error: %1")
			.arg(QString::fromStdString(v_error.getErrorMsg()));

		QtUtil::errorWithSound(this, "Tile Read Error", v_error_str);
		return;
	}

	this->appendMainObjectInfo<TileInstance>(tile);

	const QString v_tile_sz = QString("Tile Size: %1")
		.arg(FilterSettingsData::GetTileSizeName(tile->size_filter));
	m_infoLayout->addWidget(new QLabel(v_tile_sz, this));

	this->appendPartsStats();
	m_modList->updateModList();
}

void ObjectInfoGui::appendPartsStats()
{
	const QString v_part_count = QString("Part Count: %1")
		.arg(QString::number(ItemModStats::GetTotalPartCount(), 10));
	const QString v_mod_count = QString("Mod Count: %1")
		.arg(QString::number(ItemModStats::ModStorage.size(), 10));

	m_infoLayout->addWidget(new QLabel(v_part_count, this));
	m_infoLayout->addWidget(new QLabel(v_mod_count, this));
}