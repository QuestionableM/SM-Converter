#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

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

SM_MANAGED_CODE