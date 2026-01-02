#include "MtlFileWriter.hpp"

#include "ObjectDatabase/ProgCounter.hpp"
#include "UStd/UnmanagedFstream.hpp"

SM_UNMANAGED_CODE

void MtlFileWriter::Write(
	const std::wstring_view& path,
	const SMEntity::EntityTextureMap& textureMap)
{
	std::ofstream v_mtlWriter(path.data());
	if (!v_mtlWriter.is_open())
		return;

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

		const SMTextureList& tList = v_curTextureData.second.m_textures;

		if (!tList.m_nor.empty()) v_outputStr.append("\nmap_Bump " + String::ToUtf8(tList.m_nor));
		if (!tList.m_dif.empty()) v_outputStr.append("\nmap_Kd " + String::ToUtf8(tList.m_dif));
		if (!tList.m_asg.empty()) v_outputStr.append("\nmap_Ks " + String::ToUtf8(tList.m_asg));

		v_outputStr.append("\n\n");

		v_mtlWriter.write(v_outputStr.c_str(), v_outputStr.size());

		ProgCounter::ProgressValue++;
	}
}