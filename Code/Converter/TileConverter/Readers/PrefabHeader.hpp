#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/BitStream.hpp"
#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"

SM_UNMANAGED_CODE

struct PrefabDataStream
{
	std::uint32_t bitOffset;
	std::uint32_t count;
	std::uint32_t something;
	std::uint32_t bitSize;

	inline void Read(BitStream& stream)
	{
		bitOffset = stream.ReadU32();
		count = stream.ReadU32();
		something = stream.ReadU32();
		bitSize = stream.ReadU32();
	}

	inline void Reset()
	{
		bitOffset = 0;
		count = 0;
		something = 0;
		bitSize = 0;
	}
};

class PrefabHeader
{
public:
	PrefabDataStream blueprintStream;
	PrefabDataStream prefabStream;
	PrefabDataStream nodeStream;
	PrefabDataStream assetStream;
	PrefabDataStream decalStream;
	PrefabDataStream unk1Stream;
	PrefabDataStream kinematicsStream;
	PrefabDataStream unk2Stream;
	
	PrefabHeader() = default;
	~PrefabHeader() = default;
	
	inline void Read(BitStream& stream, int version)
	{
		blueprintStream.Read(stream);
		prefabStream.Read(stream);
		nodeStream.Read(stream);

		if (2 < version)
			assetStream.Read(stream);
		else
			assetStream.Reset();

		if (3 < version)
			decalStream.Read(stream);
		else
			decalStream.Reset();

		if (5 < version)
			unk1Stream.Read(stream);
		else
			unk1Stream.Reset();

		if (7 < version)
		{
			kinematicsStream.Read(stream);
			unk2Stream.Read(stream);
		}
		else
		{
			kinematicsStream.Reset();
			unk2Stream.Reset();
		}

	#if defined(DEBUG) || defined(_DEBUG)
		DebugOutL("Prefab Data:");
		if (blueprintStream.count != 0)
			DebugOutL("Blueprint Stream { offset: ", blueprintStream.bitOffset, ", count: ", blueprintStream.count, ", size: ", blueprintStream.bitSize, " }");
		if (prefabStream.count != 0)
			DebugOutL("Prefab Stream { offset: ", prefabStream.bitOffset, ", count: ", prefabStream.count, ", size: ", prefabStream.bitSize, " }");
		if (nodeStream.count != 0)
			DebugOutL("Node Stream { offset: ", nodeStream.bitOffset, ", count: ", nodeStream.count, ", size: ", nodeStream.bitSize, " }");
		if (assetStream.count != 0)
			DebugOutL("Asset Stream { offset: ", assetStream.bitOffset, ", count: ", assetStream.count, ", size: ", assetStream.bitSize, " }");
		if (decalStream.count != 0)
			DebugOutL("Decal Stream { offset: ", decalStream.bitOffset, ", count: ", decalStream.count, ", size: ", decalStream.bitSize, " }");
		if (unk1Stream.count != 0)
			DebugOutL("Unk1 Stream { offset: ", unk1Stream.bitOffset, ", count: ", unk1Stream.count, ", size: ", unk1Stream.bitSize, " }");
		if (kinematicsStream.count != 0)
			DebugOutL("Kinematic Stream { offset: ", kinematicsStream.bitOffset, ", count: ", kinematicsStream.count, ", size: ", kinematicsStream.bitSize, " }");
		if (unk2Stream.count != 0)
			DebugOutL("Unk2 Stream { offset: ", unk2Stream.bitOffset, ", count: ", unk2Stream.count, ", size: ", unk2Stream.bitSize, " }");
	#endif
	}
};

SM_MANAGED_CODE