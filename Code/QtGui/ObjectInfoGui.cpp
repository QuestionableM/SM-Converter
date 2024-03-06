#include "ObjectInfoGui.hpp"

#include "ObjectDatabase/UserDataReaders/BlueprintFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/WorldFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/TileFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/CustomGameSwitch.hpp"

#include "Converter/WorldConverter/World.hpp"
#include "Converter/Entity/Blueprint.hpp"

#include "QtGui/QtUtil.hpp"

ObjectInfoGui::ObjectInfoGui(const QString& title, const std::wstring& image, QWidget* parent)
	: QDialog(parent),
	m_mainLayout(new QVBoxLayout(this)),
	m_mainInfoLayout(new QHBoxLayout(this)),
	m_objectImage(new ImageBox(QString::fromStdWString(image))),
	m_infoLayout(new QVBoxLayout(this)),
	m_modList(new QListWidget(this))
{
	this->setWindowTitle(title);

	m_objectImage->setFixedSize(100, 100);
	m_mainLayout->addLayout(m_mainInfoLayout);
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
	this->updateModStorage();
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

ObjectInfoGui::ObjectInfoGui(WorldInstance* world, QWidget* parent)
	: ObjectInfoGui("World Info", world->preview_image, parent)
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
		const QString v_error_msg = QString("Error: %1")
			.arg(QString::fromStdWString(v_error.GetErrorMsg()));

		QtUtil::errorWithSound(this, "World Read Error", v_error_msg);
		return;
	}

	this->appendMainObjectInfo<WorldInstance>(world);
	m_infoLayout->addWidget(
		new QLabel(QString("World Size: %1x%1").arg(v_world_width), this));
	
	this->appendPartsStats();
	this->updateModStorage();
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
			.arg(QString::fromStdWString(v_error.GetErrorMsg()));

		QtUtil::errorWithSound(this, "Tile Read Error", v_error_str);
		return;
	}

	this->appendMainObjectInfo<TileInstance>(tile);

	const QString v_tile_sz = QString("Tile Size: %1")
		.arg(FilterSettingsData::GetTileSizeName(tile->v_size_filter));
	m_infoLayout->addWidget(new QLabel(v_tile_sz, this));

	this->appendPartsStats();
	this->updateModStorage();
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

void ObjectInfoGui::updateModStorage()
{
	m_modList->blockSignals(true);
	m_modList->clear();

	for (const ItemModInstance* v_mod_data : ItemModStats::ModVector)
	{
		std::wstring v_mod_name = (v_mod_data->mod != nullptr)
			? v_mod_data->mod->GetName()
			: L"UNKNOWN_MOD";

		v_mod_name.append(L" (");
		v_mod_name.append(std::to_wstring(v_mod_data->part_count));
		v_mod_name.append(L")");

		m_modList->insertItem(0, QString::fromStdWString(v_mod_name));
	}

	m_modList->blockSignals(false);
	m_modList->update();
}