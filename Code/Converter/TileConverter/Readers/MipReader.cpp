#include "Converter/ConvertSettings.hpp"
#include "MipReader.hpp"

SM_UNMANAGED_CODE

void MipReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	std::vector<Byte> v_bytes;
	if (!MipReader::Read(context, 0, v_bytes)) return;

	MipReader::Read(v_bytes, context.pTilePart);
}

bool MipReader::Read(
	TileReaderContext& context,
	int mipOrLevel,
	std::vector<Byte>& outBytes)
{
	DebugOutL(
		"MipIndex: ", context.pCellHeader->mipIndex[mipOrLevel],
		", MipCompressedSize: ", context.pCellHeader->mipCompressedSize[mipOrLevel]);

	if (!context.pReader->hasEnoughSpace(
		context.pCellHeader->mipIndex[mipOrLevel],
		context.pCellHeader->mipCompressedSize[mipOrLevel]))
	{
		DebugErrorL("Not enough space!");
		return false;
	}

	std::vector<Byte> v_bytes(context.pCellHeader->mipSize[mipOrLevel]);
	const int debugSize = Lz4::DecompressFast(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->mipIndex[mipOrLevel])),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->mipSize[mipOrLevel]);

	if (debugSize != context.pCellHeader->mipCompressedSize[mipOrLevel])
	{
		DebugErrorL("Debug Size: ", debugSize, ", header->mipCompressedSize[mipOrLevel]: ", context.pCellHeader->mipCompressedSize[mipOrLevel]);
		context.pError->setError(1, "MipReader::Read -> debugSize != header->mipCompressedSize[mipOrLevel]");
		return false;
	}

	outBytes = std::move(v_bytes);
	return true;
}

void MipReader::Read(const std::vector<Byte>& bytes, TilePart* pPart)
{
	//color and height map is 0x21 * 0x21 (33 * 33)
	constexpr std::size_t wh_mul = 0x21 * 0x21;
	constexpr std::size_t wh_mul_8 = wh_mul * 8;

	MemoryWrapper memory(bytes);

	for (std::size_t a = 0; a < wh_mul; a++)
	{
		const std::size_t mOffset = a * 8;

		pPart->m_vertexHeight[a] = memory.Object<float>(mOffset);
		pPart->m_vertexColor[a] = memory.Object<int>(mOffset + 4);
	}

	memory.ObjectsRef<long long>(pPart->m_ground.data(), wh_mul_8, 0x41 * 0x41);
}