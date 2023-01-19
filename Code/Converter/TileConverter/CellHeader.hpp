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

	std::vector<Byte> mBytes = {};

	inline void Read()
	{
		MemoryWrapper mMemory(mBytes);

		mMemory.ObjectsRef<int>(this->mipIndex.data()         , 0x0 , 6);
		mMemory.ObjectsRef<int>(this->mipCompressedSize.data(), 0x18, 6);
		mMemory.ObjectsRef<int>(this->mipSize.data()          , 0x30, 6);

		this->clutterIndex          = mMemory.Object<int>(0x48);
		this->clutterCompressedSize = mMemory.Object<int>(0x4c);
		this->clutterSize           = mMemory.Object<int>(0x50);

		mMemory.ObjectsRef<int>(this->assetListCount.data()         , 0x54, 4);
		mMemory.ObjectsRef<int>(this->assetListIndex.data()         , 0x64, 4);
		mMemory.ObjectsRef<int>(this->assetListCompressedSize.data(), 0x74, 4);
		mMemory.ObjectsRef<int>(this->assetListSize.data()          , 0x84, 4);

		this->blueprintListCount          = mMemory.Object<int>(0x94);
		this->blueprintListIndex          = mMemory.Object<int>(0x98);
		this->blueprintListCompressedSize = mMemory.Object<int>(0x9c);
		this->blueprintListSize           = mMemory.Object<int>(0xa0);

		this->nodeCount          = mMemory.Object<int>(0xa4);
		this->nodeIndex          = mMemory.Object<int>(0xa8);
		this->nodeCompressedSize = mMemory.Object<int>(0xac);
		this->nodeSize           = mMemory.Object<int>(0xb0);

		this->prefabCount          = mMemory.Object<int>(0xc4);
		this->prefabIndex          = mMemory.Object<int>(0xc8);
		this->prefabCompressedSize = mMemory.Object<int>(0xcc);
		this->prefabSize           = mMemory.Object<int>(0xd0);

		this->decalCount          = mMemory.Object<int>(0xd4);
		this->decalIndex          = mMemory.Object<int>(0xd8);
		this->decalCompressedSize = mMemory.Object<int>(0xdc);
		this->decalSize           = mMemory.Object<int>(0xe0);

		mMemory.ObjectsRef<int>(this->harvestableListCount.data()         , 0xe4 , 4);
		mMemory.ObjectsRef<int>(this->harvestableListIndex.data()         , 0xf4 , 4);
		mMemory.ObjectsRef<int>(this->harvestableListCompressedSize.data(), 0x104, 4);
		mMemory.ObjectsRef<int>(this->harvestableListSize.data()          , 0x114, 4);
	}

	inline CellHeader(const std::vector<Byte>& memory)
	{
		this->mBytes = memory;
	}

	inline std::vector<Byte> Data()
	{
		return this->mBytes;
	}
};

#pragma managed