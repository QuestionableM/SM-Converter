#include "MtlFileWriter.hpp"

#include "ObjectDatabase\ProgCounter.hpp"

#include <fstream>

#pragma unmanaged

void MtlFileWriter::Write(const std::wstring& path, const std::unordered_map<std::string, ObjectTexData>& v_data)
{
	std::ofstream v_mtl_writer(path);
	if (!v_mtl_writer.is_open())
		return;

	for (const auto& v_tex_data : v_data)
	{
		std::string output_str = "newmtl " + v_tex_data.first;
		output_str.append("\nNs 324");
		output_str.append("\nKa 1 1 1\nKd ");
		output_str.append(v_tex_data.second.TexColor.StringNormalized());
		output_str.append("\nKs 0.5 0.5 0.5");
		output_str.append("\nKe 0 0 0");
		output_str.append("\nNi 1.45");
		output_str.append("\nd 1");
		output_str.append("\nillum 2");

		const TextureList& tList = v_tex_data.second.Textures;

		if (!tList.nor.empty()) output_str.append("\nmap_Bump " + String::ToUtf8(tList.nor));
		if (!tList.dif.empty()) output_str.append("\nmap_Kd " + String::ToUtf8(tList.dif));
		if (!tList.asg.empty()) output_str.append("\nmap_Ks " + String::ToUtf8(tList.asg));

		output_str.append("\n\n");

		v_mtl_writer.write(output_str.c_str(), output_str.size());

		ProgCounter::ProgressValue++;
	}
}