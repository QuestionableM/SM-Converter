#pragma once

#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/ByteImpl.hpp"

#include <shobjidl.h>

SM_UNMANAGED_CODE

namespace File
{
	bool ReadFileBytes(const std::wstring_view& path, std::vector<Byte>& bytes);

	//Reads binary string
	bool ReadToString(const std::wstring_view& path, std::string& output);
	//Reads normal string
	bool ReadToStringNormal(const std::wstring_view& path, std::string& output);
	//File reader specifically designed to handle weird encodings
	bool ReadToStringED(const std::wstring_view& path, std::string& output);

	bool Exists(const std::wstring_view& path);
	bool CreateDirectorySafe(const std::wstring_view& path);

	bool GetFullFilePath(const std::wstring& path, std::wstring& output);
	bool GetFullFilePathLower(const std::wstring& path, std::wstring& output);

	bool IsDirectory(const std::wstring_view& path);
	bool IsRegularFile(const std::wstring_view& path);
	bool IsPathLegal(const std::wstring_view& path);
	bool IsSubPath(const std::wstring_view& parentDir, const std::wstring_view& subDir);

	bool Equivalent(const std::wstring_view& p1, const std::wstring_view& p2);
	bool CanonicalR(std::wstring& path);

	std::wstring OpenFileDialog(
		const wchar_t* title,
		FILEOPENDIALOGOPTIONS options = 0,
		LPCWSTR filter = L"All Files (*.*)\0*.*\0",
		HWND owner = NULL
	);

	bool GetAppDataPath(std::wstring& mPath);
}

SM_MANAGED_CODE