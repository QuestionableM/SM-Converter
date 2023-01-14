#pragma once

#include "Converter\Entity\Entity.hpp"

#include <string>
#include <unordered_map>

class MtlFileWriter
{
public:
	static void Write(const std::wstring& path, const std::unordered_map<std::string, ObjectTexData>& v_data);
	/*
	std::unordered_map<std::string, ObjectTexData>
	*/

private:
	MtlFileWriter() = default;
	MtlFileWriter(const MtlFileWriter&&) = delete;
	MtlFileWriter(MtlFileWriter&&) = delete;
	MtlFileWriter(MtlFileWriter&) = delete;
	~MtlFileWriter() = default;
};