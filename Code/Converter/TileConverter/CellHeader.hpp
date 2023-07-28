#pragma once

#include "UStd\UnmanagedArray.hpp"

#include "Utils\ByteImpl.hpp"
#include "Utils\Memory.hpp"

#pragma unmanaged

class CellHeader
{
public:
	/* 0x00 */ std::array<int, 6> mipIndex;
	/* 0x18 */ std::array<int, 6> mipCompressedSize;
	/* 0x30 */ std::array<int, 6> mipSize;

	/* 0x48 */ int clutterIndex;
	/* 0x4c */ int clutterCompressedSize;
	/* 0x50 */ int clutterSize;

	/* 0x54 */ std::array<int, 4> assetListCount;
	/* 0x64 */ std::array<int, 4> assetListIndex;
	/* 0x74 */ std::array<int, 4> assetListCompressedSize;
	/* 0x84 */ std::array<int, 4> assetListSize;

	/* 0x94 */ int blueprintListCount;
	/* 0x98 */ int blueprintListIndex;
	/* 0x9c */ int blueprintListCompressedSize;
	/* 0xa0 */ int blueprintListSize;

	/* 0xa4 */ int nodeCount;
	/* 0xa8 */ int nodeIndex;
	/* 0xac */ int nodeCompressedSize;
	/* 0xb0 */ int nodeSize;

	/* 0xb4 */ int scriptCount;
	/* 0xb8 */ int scriptIndex;
	/* 0xbc */ int scriptCompressedSize;
	/* 0xc0 */ int scriptSize;

	/* 0xc4 */ int prefabCount;
	/* 0xc8 */ int prefabIndex;
	/* 0xcc */ int prefabCompressedSize;
	/* 0xd0 */ int prefabSize;

	/* 0xd4 */ int decalCount;
	/* 0xd8 */ int decalIndex;
	/* 0xdC */ int decalCompressedSize;
	/* 0xe0 */ int decalSize;

	/* 0xe4 */ std::array<int, 4> harvestableListCount;
	/* 0xf4 */ std::array<int, 4> harvestableListIndex;
	/* 0x104 */std::array<int, 4> harvestableListCompressedSize;
	/* 0x114 */std::array<int, 4> harvestableListSize;

	/* 0x124 */std::array<int, 4> kinematicsListCount;
	/* 0x134 */std::array<int, 4> kinematicsListIndex;
	/* 0x144 */std::array<int, 4> kinematicsListCompressedSize;
	/* 0x154 */std::array<int, 4> kinematicsListSize;

	/* 0x164 */std::array<int, 5> some_data;
	/* 0x178 */int voxelTerrainIndex;
	/* 0x17C */int voxelTerrainCompressedSize;
	/* 0x180 */int voxelTerrainSize;

	inline CellHeader(const Byte* buffer, std::size_t size)
	{
		assert(size <= sizeof(CellHeader));

		std::memset(this, 0, sizeof(CellHeader));
		std::memcpy(this, buffer, size);
	}
};

#pragma managed