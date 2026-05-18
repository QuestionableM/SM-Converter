#include "Converter/ConvertSettings.hpp"
#include "TileReader.hpp"

bool TileReader::ReadTileHeader(
	const std::wstring& path,
	TileHeaderBaseInfo& out_header,
	ConvertError& error)
{
	std::ifstream v_tileFile(path, std::ios::binary);
	if (!v_tileFile.is_open())
	{
		error.setError(1, "TileReader::ReadTile -> Couldn't open the file");
		return false;
	}

	struct
	{
		std::uint32_t secret;
		TileHeaderBaseInfo info;
	} v_tileHeader;

	if (!v_tileFile.read(reinterpret_cast<char*>(&v_tileHeader), sizeof(v_tileHeader)).good())
	{
		error.setError(1, "TileReader::ReadTile -> Couldn't read the file");
		return false;
	}

	if (v_tileHeader.secret != 0x454C4954) // TILE - magic keyword
	{
		DebugOutL("Invalid File");
		error.setError(1, "TileHeader::ReadTile -> Invalid File");
		return false;
	}

	if (v_tileHeader.info.version > 1000000)
	{
		DebugErrorL("Invalid Version");
		error.setError(1, "TileHeader::ReadTile -> Invalid Tile Version");
		return false;
	}

	if (v_tileHeader.info.cell_header_offset != sizeof(v_tileHeader))
	{
		error.setError(1, "TileHeader::ReadTile -> Index doesn't match the cell header offset!");
		return false;
	}

	out_header = v_tileHeader.info;
	return true;
}

void TileReader::ReadTileData(TileReaderContext& context)
{
	AssetListReader::Read(context);
	PrefabReader::Read(context);
	BlueprintListReader::Read(context);
	HarvestableListReader::Read(context);
	DecalListReader::Read(context);

	if (context.tileVersion >= 11)
		KinematicsListReader::Read(context);
}

bool TileReader::ReadTile(
	const std::wstring_view& path,
	ConvertError& error,
	Tile& outTile,
	const bool counterMode)
{
	std::vector<Byte> v_fileBytes;
	if (!File::ReadFileBytes(path, v_fileBytes))
		return false;

	return TileReader::ReadTile(v_fileBytes, error, outTile, counterMode);
}

bool TileReader::ReadTile(
	const std::vector<Byte>& tileData,
	ConvertError& error,
	Tile& outTile,
	const bool counterMode)
{
	TileHeader v_header;
	if (!TileHeader::ReadTile(&v_header, tileData, error))
		return false;

	DebugOutL("TileFileVersion: ", v_header.m_data.version);
	DebugOutL("TileUuid: ", v_header.m_data.uuid.toString());
	DebugOutL("CreatorId: ", v_header.m_data.creator_id);
	DebugOutL("Size: ", v_header.m_data.width, ", ", v_header.m_data.height);
	DebugOutL("Type: ", v_header.m_data.type, "\n");
	DebugOutL("Header info:");
	DebugOutL("CellHeaderOffset: ", v_header.m_data.cell_header_offset);
	DebugOutL("CellHeadersSize: ", v_header.m_data.cell_header_size, "\n");

	const std::uint32_t v_tileVersion = v_header.m_data.version;
	if (v_tileVersion > 13)
	{
		error.setError(1, "Unsupported Tile Version: ", v_tileVersion);
		return false;
	}

	if (v_header.m_data.width != v_header.m_data.height)
	{
		error.setError(1, "Weird tile dimensions: ", v_header.m_data.width, ',', v_header.m_data.height);
		return false;
	}

	MemoryWrapper v_reader(tileData);
	Tile v_newTile(v_header);

	TileReaderContext v_context;
	v_context.pReader = &v_reader;
	v_context.pError = &error;
	v_context.tileVersion = v_tileVersion;
	v_context.counterMode = counterMode;

	for (std::uint32_t y = 0; y < v_header.m_data.height; y++)
	{
		for (std::uint32_t x = 0; x < v_header.m_data.width; x++)
		{
			v_context.pCellHeader = &v_header.GetCellHeader(x, y);
			v_context.pTilePart = v_newTile.GetPart(x, y);

			if (!counterMode && v_newTile.m_hasTerrain) // mod counter doesn't need any information about the clutter, so it is skipped to improve performance
			{
				MipReader::Read(v_context);
				if (error) goto escape_loop;

				ClutterReader::Read(v_context);
				if (error) goto escape_loop;
			}

			TileReader::ReadTileData(v_context);
			if (error) goto escape_loop;
		}
	}

escape_loop:
	if (error)
		return false;

	outTile = std::move(v_newTile);
	return true;
}