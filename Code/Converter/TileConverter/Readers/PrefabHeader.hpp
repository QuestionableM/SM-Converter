#pragma once

#include "Utils\BitStream.hpp"
#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"

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

	int count_0x64;
	int has_0x6c;

	int count_0x74;
	int has_0x7c;
	
	int version;
	
	inline PrefabHeader(const int& version)
	{
		this->version = version;
	}
	
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

		if (2 < version)
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

		if (3 < version)
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

		if (5 < version)
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

		if (7 < version)
		{
			stream.Move(4 * 8);
			count_0x64 = stream.ReadInt();
			stream.Move(4 * 8);
			has_0x6c = stream.ReadInt();

			stream.Move(4 * 8);
			count_0x74 = stream.ReadInt();
			stream.Move(4 * 8);
			has_0x7c = stream.ReadInt();
		}
		else
		{
			count_0x64 = 0;
			has_0x6c   = 0;

			count_0x74 = 0;
			has_0x7c   = 0;
		}

		DebugOutL("Prefab Data:");
		DebugOutL("Blueprint Count: ", blueprintCount);
		DebugOutL("Has Blueprints: ", hasBlueprints);
		DebugOutL("Prefab Count: ", prefabCount);
		DebugOutL("Has Prefabs: ", hasPrefabs);
		DebugOutL("Node Count: ", nodeCount);
		DebugOutL("Has Nodes: ", hasNodes);
		DebugOutL("Asset Count: ", assetCount);
		DebugOutL("Has Assets: ", hasAssets);
		DebugOutL("Decals Count: ", decalsCount);
		DebugOutL("Has Decals: ", hasDecals);
		DebugOutL("\nUnknown variables:");
		DebugOutL("Count_0x54: ", count_0x54);
		DebugOutL("Has_0x5c: ", has_0x5c);
		DebugOutL("Count_0x64: ", count_0x64);
		DebugOutL("Has_0x6c: ", has_0x6c);
		DebugOutL("Count_0x74: ", count_0x74);
		DebugOutL("Has_0x7c: ", has_0x7c);
	}
};