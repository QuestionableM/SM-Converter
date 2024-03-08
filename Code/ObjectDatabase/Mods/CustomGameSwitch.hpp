#pragma once

#include "ObjectDatabase/Mods/CustomGameMod.hpp"
#include "ObjectDatabase/KeywordReplacer.hpp"
#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Console.hpp"

SM_UNMANAGED_CODE

template<bool t_modify_blocks_only, bool t_sets_content_key>
class SMModCustomGameSwitch
{
public:
	inline SMModCustomGameSwitch()
	{
		if constexpr (!t_modify_blocks_only) {
			m_Assets = SMModObjectStorage<AssetData>::StaticStorage;
			m_Harvestables = SMModObjectStorage<HarvestableData>::StaticStorage;
			m_Kinematics = SMModObjectStorage<KinematicData>::StaticStorage;
		}

		m_Blocks = SMModObjectStorage<BlockData>::StaticStorage;
		m_Parts = SMModObjectStorage<PartData>::StaticStorage;
	}

	inline ~SMModCustomGameSwitch()
	{
		DebugOutL(0b1011_fg, "Setting the normal content back...");

		if constexpr (!t_modify_blocks_only) {
			SMModObjectStorage<AssetData>::StaticStorage = m_Assets;
			SMModObjectStorage<HarvestableData>::StaticStorage = m_Harvestables;
			SMModObjectStorage<KinematicData>::StaticStorage = m_Kinematics;
		}

		SMModObjectStorage<BlockData>::StaticStorage = m_Blocks;
		SMModObjectStorage<PartData>::StaticStorage = m_Parts;

		if constexpr (t_sets_content_key) {
			KeywordReplacer::ClearModKeys();
		}
	}

	inline void MergeContent(CustomGame* v_cg_mod) const
	{
		DebugOutL(0b1011_fg, "Merging custom game content...");

		SMModObjectStorage<BlockData>::Clear();
		SMModObjectStorage<PartData>::Clear();

		if constexpr (!t_modify_blocks_only) {
			SMModObjectStorage<AssetData>::Clear();
			SMModObjectStorage<HarvestableData>::Clear();
			SMModObjectStorage<KinematicData>::Clear();
		}

		if (v_cg_mod->m_shouldUseGameContent)
		{
			//Fill the storage with base game content and then merge custom game into that data

			if constexpr (!t_modify_blocks_only) {
				SMModObjectStorage<AssetData>::StaticStorage = SMMod::GameDataMod->m_Assets.DynamicStorage;
				SMModObjectStorage<HarvestableData>::StaticStorage = SMMod::GameDataMod->m_Harvestables.DynamicStorage;
				SMModObjectStorage<KinematicData>::StaticStorage = SMMod::GameDataMod->m_Kinematics.DynamicStorage;

				SMMod::MergeMaps<AssetData, true>(SMModObjectStorage<AssetData>::StaticStorage, v_cg_mod->m_Assets.DynamicStorage);
				SMMod::MergeMaps<HarvestableData, true>(SMModObjectStorage<HarvestableData>::StaticStorage, v_cg_mod->m_Harvestables.DynamicStorage);
				SMMod::MergeMaps<KinematicData, true>(SMModObjectStorage<KinematicData>::StaticStorage, v_cg_mod->m_Kinematics.DynamicStorage);
			}

			SMModObjectStorage<BlockData>::StaticStorage = SMMod::GameDataMod->m_Blocks.DynamicStorage;
			SMModObjectStorage<PartData>::StaticStorage = SMMod::GameDataMod->m_Parts.DynamicStorage;

			SMMod::MergeMaps<BlockData, true>(SMModObjectStorage<BlockData>::StaticStorage, v_cg_mod->m_Blocks.DynamicStorage);
			SMMod::MergeMaps<PartData, true>(SMModObjectStorage<PartData>::StaticStorage, v_cg_mod->m_Parts.DynamicStorage);
		}
		else
		{
			//Fill the storage with custom game content only

			if constexpr (!t_modify_blocks_only) {
				SMModObjectStorage<AssetData>::StaticStorage = v_cg_mod->m_Assets.DynamicStorage;
				SMModObjectStorage<HarvestableData>::StaticStorage = v_cg_mod->m_Harvestables.DynamicStorage;
				SMModObjectStorage<KinematicData>::StaticStorage = v_cg_mod->m_Kinematics.DynamicStorage;
			}

			SMModObjectStorage<BlockData>::StaticStorage = v_cg_mod->m_Blocks.DynamicStorage;
			SMModObjectStorage<PartData>::StaticStorage = v_cg_mod->m_Parts.DynamicStorage;
		}

		if (v_cg_mod->m_shouldUseUserMods)
		{
			DebugOutL(0b1011_fg, "Merging mods into the custom game content...");

			for (SMMod* v_cur_mod : SMMod::GetAllMods())
			{
				if constexpr (!t_modify_blocks_only) {
					SMMod::MergeMaps<AssetData, false>(SMModObjectStorage<AssetData>::StaticStorage, v_cur_mod->m_Assets.DynamicStorage);
					SMMod::MergeMaps<HarvestableData, false>(SMModObjectStorage<HarvestableData>::StaticStorage, v_cur_mod->m_Harvestables.DynamicStorage);
					SMMod::MergeMaps<KinematicData, false>(SMModObjectStorage<KinematicData>::StaticStorage, v_cur_mod->m_Kinematics.DynamicStorage);
				}

				SMMod::MergeMaps<BlockData, false>(SMModObjectStorage<BlockData>::StaticStorage, v_cur_mod->m_Blocks.DynamicStorage);
				SMMod::MergeMaps<PartData, false>(SMModObjectStorage<PartData>::StaticStorage, v_cur_mod->m_Parts.DynamicStorage);
			}
		}

		if constexpr (t_sets_content_key) {
			v_cg_mod->SetContentKey();
		}
	}

private:
	std::unordered_map<SMUuid, BlockData*> m_Blocks;
	std::unordered_map<SMUuid, PartData*> m_Parts;
	std::unordered_map<SMUuid, AssetData*> m_Assets;
	std::unordered_map<SMUuid, HarvestableData*> m_Harvestables;
	std::unordered_map<SMUuid, KinematicData*> m_Kinematics;
};

SM_MANAGED_CODE