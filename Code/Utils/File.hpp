#pragma once

#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"

#include <shobjidl.h>

#pragma unmanaged

namespace File
{
	bool ReadFileBytes(const std::wstring& path, std::vector<Byte>& bytes);

	//Reads binary string
	bool ReadToString(const std::wstring& path, std::string& r_output);
	//Reads normal string
	bool ReadToStringNormal(const std::wstring& path, std::string& r_output);

	bool Exists(const std::wstring& path);
	bool CreateDirectorySafe(const std::wstring& path);

	bool GetFullFilePath(const std::wstring& path, std::wstring& v_output);
	bool GetFullFilePathLower(const std::wstring& path, std::wstring& v_output);

	bool IsDirectory(const std::wstring& path);
	bool IsRegularFile(const std::wstring& path);
	bool IsPathLegal(const std::wstring& path);
	bool IsSubPath(const std::wstring& parent_dir, const std::wstring& sub_dir);

	bool Equivalent(const std::wstring& p1, const std::wstring& p2);

	std::wstring OpenFileDialog(
		const std::wstring& title,
		FILEOPENDIALOGOPTIONS options = 0,
		LPCWSTR filter = L"All Files (*.*)\0*.*\0",
		HWND owner = NULL
	);

	bool GetAppDataPath(std::wstring& mPath);
}

#pragma managed