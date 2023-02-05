#pragma once

#include "Utils\BitStream.hpp"
#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"

#pragma unmanaged

class PrefabHeader
{
public:
	int blueprintCount;
	int hasBlueprints;
	
	int prefabCount;
	int hasPrefabs;
	
	int nodeCount;
	int hasNodes;
	
	int assetCount;
	int hasAssets;
	
	int decalsCount;
	int hasDecals;
	
	int count_0x54;
	int has_0x5c;

	int kinematicsCount;
	int hasKinematics;

	int count_0x74;
	int has_0x7c;
	
	PrefabHeader() = default;
	~PrefabHeader() = default;
	
	template<int t_pref_version>
	inline void Read(BitStream& stream)
	{
		stream.Move(4 * 8);
		blueprintCount = stream.ReadInt();
		stream.Move(4 * 8);
		hasBlueprints = stream.ReadInt();

		stream.Move(4 * 8);
		prefabCount = stream.ReadInt();
		stream.Move(4 * 8);
		hasPrefabs = stream.ReadInt();

		stream.Move(4 * 8);
		nodeCount = stream.ReadInt();
		stream.Move(4 * 8);
		hasNodes = stream.ReadInt();

		if constexpr (2 < t_pref_version)
		{
			stream.Move(4 * 8);
			assetCount = stream.ReadInt();
			stream.Move(4 * 8);
			hasAssets = stream.ReadInt();
		}
		else
		{
			assetCount = 0;
			hasAssets  = 0;
		}

		if constexpr (3 < t_pref_version)
		{
			stream.Move(4 * 8);
			decalsCount = stream.ReadInt();
			stream.Move(4 * 8);
			hasDecals = stream.ReadInt();
		}
		else
		{
			decalsCount = 0;
			hasDecals = 0;
		}

		if constexpr (5 < t_pref_version)
		{
			stream.Move(4 * 8);
			count_0x54 = stream.ReadInt();
			stream.Move(4 * 8);
			has_0x5c = stream.ReadInt();
		}
		else
		{
			count_0x54 = 0;
			has_0x5c   = 0;
		}

		if constexpr (7 < t_pref_version)
		{
			stream.Move(4 * 8);
			kinematicsCount = stream.ReadInt();
			stream.Move(4 * 8);
			hasKinematics = stream.ReadInt();

			stream.Move(4 * 8);
			count_0x74 = stream.ReadInt();
			stream.Move(4 * 8);
			has_0x7c = stream.ReadInt();
		}
		else
		{
			kinematicsCount = 0;
			hasKinematics   = 0;

			count_0x74 = 0;
			has_0x7c   = 0;
		}

	#if defined(DEBUG) || defined(_DEBUG)
		DebugOutL("Prefab Data:");
		if (hasBlueprints != 0)
			DebugOutL("Blueprint Count: ", blueprintCount);
		if (hasPrefabs != 0)
			DebugOutL("Prefab Count: ", prefabCount);
		if (hasNodes != 0)
			DebugOutL("Node Count: ", nodeCount);
		if (hasAssets != 0)
			DebugOutL("Asset Count: ", assetCount);
		if (hasDecals != 0)
			DebugOutL("Decals Count: ", decalsCount);
		if (has_0x5c != 0)
			DebugOutL("Count_0x54: ", count_0x54);
		if (hasKinematics != 0)
			DebugOutL("Kinematics Count: ", kinematicsCount);
		if (has_0x7c != 0)
			DebugOutL("Count_0x74: ", count_0x74);
	#endif
	}
};

#pragma managed