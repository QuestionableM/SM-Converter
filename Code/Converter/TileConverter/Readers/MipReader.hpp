#pragma once

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"

#include "UStd/UnmanagedSstream.hpp"
#include "UStd/UnmanagedIomanip.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class MipReader
{
	MipReader() = default;

public:
	inline static void Read(const CellHeader& header, MemoryWrapper& reader, TilePart* pPart, ConvertError& error)
	{
		if (error) return;

		std::vector<Byte> v_bytes;
		if (!MipReader::Read(header, 0, reader, error, v_bytes)) return;

		MipReader::Read(v_bytes, pPart);
	}

	inline static bool Read(
		const CellHeader& header,
		int mipOrLevel,
		MemoryWrapper& reader,
		ConvertError& error,
		std::vector<Byte>& out_bytes)
	{
		DebugOutL("MipIndex: ", header.mipIndex[mipOrLevel], ", MipCompressedSize: ", header.mipCompressedSize[mipOrLevel]);

		if (!reader.hasEnoughSpace(header.mipIndex[mipOrLevel], header.mipCompressedSize[mipOrLevel]))
		{
			DebugErrorL("Not enough space!");
			return false;
		}

		std::vector<Byte> v_bytes(header.mipSize[mipOrLevel]);
		const int debugSize = Lz4::DecompressFast(
			reinterpret_cast<const char*>(reader.getPointer(header.mipIndex[mipOrLevel])),
			reinterpret_cast<char*>(v_bytes.data()),
			header.mipSize[mipOrLevel]);

		if (debugSize != header.mipCompressedSize[mipOrLevel])
		{
			DebugErrorL("Debug Size: ", debugSize, ", header->mipCompressedSize[mipOrLevel]: ", header.mipCompressedSize[mipOrLevel]);
			error.setError(1, "MipReader::Read -> debugSize != header->mipCompressedSize[mipOrLevel]");
			return false;
		}

		out_bytes = std::move(v_bytes);
		return true;
	}

	inline static void Read(const std::vector<Byte>& bytes, TilePart* pPart)
	{
		//color and height map is 0x21 * 0x21 (33 * 33)
		constexpr std::size_t wh_mul = 0x21 * 0x21;
		constexpr std::size_t wh_mul_8 = wh_mul * 8;

		MemoryWrapper memory(bytes);

		for (std::size_t a = 0; a < wh_mul; a++)
		{
			const std::size_t mOffset = a * 8;

			pPart->m_vertexHeight[a] = memory.Object<float>(mOffset);
			pPart->m_vertexColor [a] = memory.Object<int>(mOffset + 4);
		}

		memory.ObjectsRef<long long>(pPart->m_ground.data(), wh_mul_8, 0x41 * 0x41);
	}
};

SM_MANAGED_CODE