#include "Lz4Lib.hpp"

#include <lz4/lz4.h>

#pragma warning(push)
#pragma warning(disable : 4996)

int Lz4::DecompressFast(const char* src, char* dst, int originalSize)
{
	return LZ4_decompress_fast(src, dst, originalSize);
}

#pragma warning(pop)