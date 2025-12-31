#include "CharacterConverter.hpp"

#include "ObjectDatabase\Mods\Mod.hpp"

SM_UNMANAGED_CODE

#include <assimp\postprocess.h>
#include <assimp\Exporter.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>

void CharacterConverter::ConvertToModel(const UserCharacterData& v_data, const std::wstring& filename, ConvertError& v_error)
{
	static const char* v_category_names[] =
	{
		"Face" , "Hair"  , "FacialHair",
		"Torso", "Gloves", "Shoes",
		"Legs" , "Hats"  , "Backpack"
	};

	static_assert((sizeof(UserCharacterData::garments) / sizeof(SMUuid)) == (sizeof(v_category_names) / sizeof(const char*)),
		"Amount of garments should be equal to the amount of categories");

	DebugOutL("Is female: ", v_data.is_female);

	Assimp::Importer v_importer;
	for (std::size_t a = 0; a < (sizeof(v_category_names) / sizeof(const char*)); a++)
	{
		const SMUuid& v_cur_uuid = v_data.garments[a];
		if (v_cur_uuid.isNil()) continue;

		GarmentData* v_garment = SMMod::GetGarment(v_category_names[a], v_data.garments[a]);
		if (!v_garment) continue;

		const std::wstring& v_cur_mesh = v_data.is_female ? v_garment->m_female_mesh : v_garment->m_male_mesh;
		const SMSubMeshBase* v_sub_mesh_data = v_data.is_female ? v_garment->m_female_data : v_garment->m_male_data;

		CharacterConverter::ReadFromPath(v_cur_mesh);
	}
}

inline void output_bone_metadata(aiNode* v_node)
{
	if (!v_node->mMetaData)
		return;

	for (unsigned int a = 0; a < v_node->mMetaData->mNumProperties; a++)
	{
		const aiString& v_key = v_node->mMetaData->mKeys[a];
		const aiMetadataEntry& v_entry = v_node->mMetaData->mValues[a];

		DebugOut(v_key.C_Str(), ": ");

		switch (v_entry.mType)
		{
		case aiMetadataType::AI_AISTRING:
			{
				const aiString& v_str = *reinterpret_cast<aiString*>(v_entry.mData);
				DebugOutL(v_str.C_Str());
				break;
			}
		case aiMetadataType::AI_AIVECTOR3D:
			{
				const aiVector3D& v_vec3 = *reinterpret_cast<aiVector3D*>(v_entry.mData);
				DebugOutL("{ ", v_vec3.x, ", ", v_vec3.y, ", ", v_vec3.z, " }");
				break;
			}
		case aiMetadataType::AI_BOOL:
			DebugOutL(*reinterpret_cast<bool*>(v_entry.mData));
			break;
		case aiMetadataType::AI_DOUBLE:
			DebugOutL(*reinterpret_cast<double*>(v_entry.mData));
			break;
		case aiMetadataType::AI_FLOAT:
			DebugOutL(*reinterpret_cast<float*>(v_entry.mData));
			break;
		case aiMetadataType::AI_INT32:
			DebugOutL(*reinterpret_cast<int*>(v_entry.mData));
			break;
		case aiMetadataType::AI_UINT64:
			DebugOutL(*reinterpret_cast<unsigned long long*>(v_entry.mData));
			break;
		default:
			DebugOut("Other type");
			break;
		}
	}
}

void traverse_the_bone_tree(aiNode* v_node)
{
	DebugOutL("Node: ", v_node->mName.C_Str());
	output_bone_metadata(v_node);

	for (unsigned int a = 0; a < v_node->mNumChildren; a++)
		traverse_the_bone_tree(v_node->mChildren[a]);
}

void CharacterConverter::ReadFromPath(const std::wstring& v_path)
{
	//RootNode
		//Armature
			//Bones

	Assimp::Importer v_importer;
	const aiScene* v_scene = v_importer.ReadFile(String::ToUtf8(v_path).c_str(), 0);
	if (!v_scene || !v_scene->mRootNode) return;

	traverse_the_bone_tree(v_scene->mRootNode);

	aiMesh* v_cur_mesh = v_scene->mMeshes[0];

	aiScene* v_exported_model = new aiScene();

	aiNode* v_root_node = new aiNode();
	v_root_node->mName = "RootNode";
	v_root_node->mNumChildren = 1;
	v_root_node->mChildren = new aiNode*[v_root_node->mNumChildren];
	v_root_node->mTransformation = aiMatrix4x4();

	aiNode* v_armature_node = new aiNode();
	v_armature_node->mName = "Armature";

	v_root_node->mChildren[0] = v_armature_node;
	/*v_exported_model->mNumMeshes = 1;
	v_exported_model->mMeshes = new aiMesh*[v_exported_model->mNumMeshes];

	v_exported_model->mNumMaterials = 1;
	v_exported_model->mMaterials = new aiMaterial*[v_exported_model->mNumMaterials];
	v_exported_model->mMaterials[0] = new aiMaterial();

	v_exported_model->mRootNode = new aiNode();
	v_exported_model->mRootNode->mMeshes = new unsigned int[v_exported_model->mNumMeshes];
	v_exported_model->mRootNode->mMeshes[0] = 0;
	v_exported_model->mRootNode->mNumMeshes = v_exported_model->mNumMeshes;
	v_exported_model->mRootNode->mName = "root_test";

	aiMesh* v_new_mesh = new aiMesh();
	v_new_mesh->mNumBones = v_cur_mesh->mNumBones;
	v_new_mesh->mBones = new aiBone*[v_new_mesh->mNumBones];
	v_new_mesh->mName = "mesh_test";

	v_exported_model->mMeshes[0] = v_new_mesh;

	for (unsigned int a = 0; a < v_new_mesh->mNumBones; a++)
	{
		aiBone* v_cpy_bone = v_scene->mMeshes[0]->mBones[a];
		aiBone* v_new_bone = new aiBone();
		v_new_bone->mArmature = nullptr;
		v_new_bone->mName = v_cpy_bone->mName;
		v_new_bone->mNode = nullptr;
		v_new_bone->mNumWeights = 0;
		v_new_bone->mWeights = nullptr;
		v_new_bone->mOffsetMatrix = v_cpy_bone->mOffsetMatrix;

		v_new_mesh->mBones[a] = v_new_bone;
	}*/

	Assimp::Exporter v_exporter;
	v_exporter.Export(v_exported_model, "fbx", "../test_shit.fbx", aiProcess_PopulateArmatureData);
	v_exporter.Export(v_exported_model, "collada", "../test_shit.dae", aiProcess_PopulateArmatureData);
}