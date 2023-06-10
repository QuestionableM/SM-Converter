#pragma once

#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

namespace Crypt
{
	enum class HashType
	{
		Sha1,
		Sha256,
		Md5
	};

	std::string GetHashString(const void* pData, std::size_t mDataSize, HashType mType);
}

#pragma managed