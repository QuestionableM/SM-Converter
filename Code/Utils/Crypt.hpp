#pragma once

#include "Utils\UnmanagedString.hpp"

#pragma unmanaged

namespace Crypt
{
	enum class HashType
	{
		Sha1,
		Sha256,
		Md5
	};

	std::string GetHashString(const void* pData, const std::size_t& mDataSize, const HashType& mType);
}

#pragma managed