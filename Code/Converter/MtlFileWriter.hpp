#pragma once

#include "Converter/Entity/Entity.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class MtlFileWriter
{
public:
	static void Write(const std::wstring_view& path, const SMEntity::EntityTextureMap& textureMap);

private:
	MtlFileWriter() = default;
	MtlFileWriter(const MtlFileWriter&) = delete;
	MtlFileWriter(MtlFileWriter&&) = delete;
	~MtlFileWriter() = default;
};

SM_MANAGED_CODE