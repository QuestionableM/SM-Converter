#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/Readers/PrefabHeader.hpp"
#include "Converter/Entity/Entity.hpp"
#include "Converter/ConvertError.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#define STR_WIDE(n) L ## n
#define STR_WIDE2(n) STR_WIDE(#n)
#define STR_WIDE3(n) STR_WIDE(n)

#define STR_INDIR(A) #A
#define STR(A) STR_INDIR(A)

#define THROW_PREFAB_ERROR(expression, error) \
	if ((expression)) { \
		DebugErrorL("Prefab Error: ", #expression); \
		error->setError(1, __FUNCTION__ "(" STR(__LINE__) ") -> " #expression); \
		return; \
	}

class SMPrefab;

class PrefabFileReader
{
	PrefabFileReader() = default;

public:
	static SMPrefab* Read(
		const std::wstring_view& path,
		const std::wstring_view& flag,
		const SMEntityTransform& transform,
		ConvertError& error,
		const bool counterMode);

	static SMPrefab* Read(
		const std::vector<Byte>& bytes,
		const std::wstring_view& ppath,
		const std::wstring_view& pflag,
		const SMEntityTransform& transform,
		ConvertError& error,
		const bool counterMode);

	static void ReadPrefabHeader(PrefabReaderContext& context);

	static int ReadBlueprints(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static int ReadPrefabs(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static void ReadNodes(
		PrefabReaderContext& context,
		SMPrefab* prefab,
		int count);

	static int ReadAssets(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static int ReadDecals(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static int Read_248(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static int ReadKinematics(
		PrefabReaderContext& context,
		PrefabHeader& header);

	static int Read_2(
		PrefabReaderContext& context,
		PrefabHeader& header);
};

SM_MANAGED_CODE