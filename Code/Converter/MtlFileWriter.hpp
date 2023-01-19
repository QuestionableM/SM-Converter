#pragma once

#include "Converter\Entity\Entity.hpp"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

class MtlFileWriter
{
public:
	static void Write(const std::wstring& path, const std::unordered_map<std::string, ObjectTexData>& v_data);

private:
	MtlFileWriter() = default;
	MtlFileWriter(const MtlFileWriter&&) = delete;
	MtlFileWriter(MtlFileWriter&&) = delete;
	MtlFileWriter(MtlFileWriter&) = delete;
	~MtlFileWriter() = default;
};

#pragma managed