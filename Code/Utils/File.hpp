#pragma once

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"

#include <vector>
#include <string>

#include <shobjidl.h>

namespace File
{
	std::vector<Byte> ReadFileBytes(const std::wstring& path);

	//Reads binary string
	bool ReadToString(const std::wstring& path, std::string& r_output);
	//Reads normal string
	bool ReadToStringNormal(const std::wstring& path, std::string& r_output);

	bool Exists(const std::wstring& path);
	bool CreateDirectorySafe(const std::wstring& path);

	bool Equivalent(const std::wstring& p1, const std::wstring& p2);

	std::wstring OpenFileDialog(
		const std::wstring& title,
		FILEOPENDIALOGOPTIONS options = 0,
		LPCWSTR filter = L"All Files (*.*)\0*.*\0",
		HWND owner = NULL
	);

	bool GetAppDataPath(std::wstring& mPath);
}