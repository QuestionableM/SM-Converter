#include "MtlFileWriter.hpp"

#include "ObjectDatabase/ProgCounter.hpp"
#include "UStd/UnmanagedFstream.hpp"

#include "Utils/WineDetect.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

void MtlFileWriter::FixPathsForLinux(SMEntity::EntityTextureMap& textureMap)
{
	// Go through the mtl file and fix the paths for Linux systems
	// All the paths in SM Converter are lowercase by default since Windows is case insensitive
	if (!WineDetect::IsRunningUnderWine())
		return;

	for (auto& v_iter : textureMap)
	{
		for (size_t a = 0; a < 3; a++)
		{
			auto& v_curTex = v_iter.second.m_textures.getStringRef(a);
			if (v_curTex.empty()) continue;

			// Get the true path with proper character casing
			File::CanonicalR(v_curTex);
			if (v_curTex.size() < 2) continue;

			// Get rid of the drive letter at the start of the path
			if (std::isalpha(v_curTex[0]) && v_curTex[1] == L':')
				v_curTex.erase(v_curTex.begin(), v_curTex.begin() + 2);
		}
	}
}

void MtlFileWriter::Write(
	const std::wstring_view& path,
	SMEntity::EntityTextureMap& textureMap)
{
	std::ofstream v_mtlWriter(path.data());
	if (!v_mtlWriter.is_open())
		return;

	MtlFileWriter::FixPathsForLinux(textureMap);
	std::string v_outputStr;

	for (const auto& v_curTextureData : textureMap)
	{
		v_outputStr.append("newmtl ");
		v_outputStr.append(v_curTextureData.first);
		v_outputStr.append("\nNs 324");
		v_outputStr.append("\nKa 1 1 1\nKd ");
		v_outputStr.append(v_curTextureData.second.m_texColor.StringNormalized());
		v_outputStr.append("\nKs 0.5 0.5 0.5");
		v_outputStr.append("\nKe 0 0 0");
		v_outputStr.append("\nNi 1.45");
		v_outputStr.append("\nd 1");
		v_outputStr.append("\nillum 2");

		const SMTextureList& v_list = v_curTextureData.second.m_textures;
		if (!v_list.m_nor.empty()) v_outputStr.append("\nmap_Bump " + String::ToUtf8(v_list.m_nor));
		if (!v_list.m_dif.empty()) v_outputStr.append("\nmap_Kd "   + String::ToUtf8(v_list.m_dif));
		if (!v_list.m_asg.empty()) v_outputStr.append("\nmap_Ks "   + String::ToUtf8(v_list.m_asg));

		v_outputStr.append("\n\n");

		v_mtlWriter.write(v_outputStr.c_str(), v_outputStr.size());

		ProgCounter::ProgressValue++;
	}
}