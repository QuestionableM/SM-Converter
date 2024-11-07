#include "File.hpp"

#include "UStd/UnmanagedFilesystem.hpp"
#include "UStd/UnmanagedFstream.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/Console.hpp"
#include "Utils/String.hpp"

namespace fs = std::filesystem;

// Uses extremely fast memory mapping instead of std::ifstream
#define USE_MEM_MAPPED_FILES

SM_UNMANAGED_CODE

#if defined(USE_MEM_MAPPED_FILES)
#	include "Utils/MemoryMapped.h"
#endif

#include <ShlObj.h>

bool File::ReadFileBytes(const std::wstring& path, std::vector<Byte>& bytes)
{
#if defined(USE_MEM_MAPPED_FILES)
	MemoryMapped v_inputFile(path, MemoryMapped::WholeFile, MemoryMapped::SequentialScan);
	if (!v_inputFile.isValid()) return false;

	bytes.resize(v_inputFile.size());
	std::memcpy(bytes.data(), v_inputFile.getData(), bytes.size());
#else
	std::ifstream input_file(path, std::ios::binary | std::ios::ate);
	if (!input_file.is_open()) return false;

	bytes.resize(static_cast<std::size_t>(input_file.tellg()));
	input_file.seekg(0, std::ios::beg);

	input_file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
#endif

	return !bytes.empty();
}

bool File::ReadToString(const std::wstring_view& path, std::string& r_output)
{
#if defined(USE_MEM_MAPPED_FILES)
	MemoryMapped v_inputFile(path, MemoryMapped::WholeFile, MemoryMapped::SequentialScan);
	if (!v_inputFile.isValid()) return false;

	r_output.resize(v_inputFile.size());
	std::memcpy(r_output.data(), v_inputFile.getData(), r_output.size());
#else
	std::ifstream input_file(path.data(), std::ios::binary | std::ios::ate);
	if (!input_file.is_open()) return false;

	r_output.resize(input_file.tellg());
	input_file.seekg(0, std::ios::beg);

	input_file.read(r_output.data(), r_output.size());
#endif

	return true;
}

bool File::ReadToStringNormal(const std::wstring& path, std::string& r_output)
{
	std::ifstream v_input_file(path, std::ios::ate);
	if (!v_input_file.is_open()) return false;

	r_output.resize(v_input_file.tellg());
	v_input_file.seekg(0, std::ios::beg);

	v_input_file.read(r_output.data(), r_output.size());
	v_input_file.close();

	return true;
}

bool File::ReadToStringED(const std::wstring& path, std::string& r_output)
{
#if defined(USE_MEM_MAPPED_FILES)
	MemoryMapped v_inputFile(path, MemoryMapped::WholeFile, MemoryMapped::SequentialScan);
	if (!v_inputFile.isValid() || v_inputFile.size() == 0) return false;

	const char v_encodingBuffer = *reinterpret_cast<const char*>(v_inputFile.getData());
	const std::size_t v_fileOffset = (v_encodingBuffer < 0) ? 3 : 0;

	if (v_fileOffset >= v_inputFile.size()) return false;

	r_output.resize(v_inputFile.size() - v_fileOffset);
	std::memcpy(r_output.data(), v_inputFile.getData() + v_fileOffset, r_output.size() - v_fileOffset);
#else
	std::ifstream v_input_file(path, std::ios::binary | std::ios::ate);
	if (!v_input_file.is_open()) return false;

	const std::size_t v_file_size = static_cast<std::size_t>(v_input_file.tellg());
	v_input_file.seekg(0, std::ios::beg);

	if (v_file_size == 0 || v_file_size == std::size_t(-1))
	{
		v_input_file.close();
		return false;
	}

	char v_encoding_buffer;
	v_input_file.read(&v_encoding_buffer, 1);

	// Guess if the file has an encoding which we want to skip
	const std::size_t v_file_offset = (v_encoding_buffer < 0) ? 3 : 0;
	if (v_file_size <= v_file_offset)
	{
		v_input_file.close();
		return false;
	}

	v_input_file.seekg(v_file_offset, std::ios::beg);
	r_output.resize(v_file_size - v_file_offset);

	v_input_file.read(r_output.data(), r_output.size());
	v_input_file.close();
#endif

	return true;
}

bool File::Exists(const std::wstring& path)
{
	std::error_code ec;
	const bool exists = fs::exists(path, ec);

	return (!ec && exists);
}

bool File::CreateDirectorySafe(const std::wstring& path)
{
	std::error_code e_error;
	const bool exists = fs::exists(path, e_error);
	const bool exists_correct = (!e_error && exists);

	std::error_code f_error;
	const bool file_created = fs::create_directory(path, f_error);
	const bool file_correct = (!f_error && file_created);

	return (exists_correct || file_correct);
}

static wchar_t g_fullPathBuffer[512] = {};

bool File::GetFullFilePath(const std::wstring& path, std::wstring& v_output)
{
	const DWORD v_pathNameSz = GetFullPathNameW(
		path.c_str(),
		sizeof(g_fullPathBuffer) / sizeof(wchar_t),
		g_fullPathBuffer,
		nullptr
	);

	if (v_pathNameSz == 0) return false;

	v_output.assign(g_fullPathBuffer, v_pathNameSz);
	return true;
}

bool File::GetFullFilePathLower(const std::wstring& path, std::wstring& v_output)
{
	if (!File::GetFullFilePath(path, v_output))
		return false;

	String::ToLowerR(v_output);
	return true;
}

bool File::IsDirectory(const std::wstring& path)
{
	std::error_code v_error;
	const bool v_exists = fs::is_directory(path);

	return (!v_error && v_exists);
}

bool File::IsRegularFile(const std::wstring& path)
{
	std::error_code v_error;
	const bool v_exists = fs::is_regular_file(path);

	return (!v_error && v_exists);
}

bool File::IsPathLegal(const std::wstring& path)
{
	std::error_code v_error;
	const bool v_exists = fs::exists(path, v_error);

	return !v_error;
}

bool File::IsSubPath(const std::wstring& parent_dir, const std::wstring& sub_dir)
{
	std::error_code v_ec;
	const std::wstring v_relative = std::filesystem::relative(sub_dir, parent_dir, v_ec);

	if (v_ec) return false;

	return v_relative.size() == 1 || (v_relative[0] != L'.' && v_relative[1] != L'.');
}

bool File::Equivalent(const std::wstring& p1, const std::wstring& p2)
{
	std::error_code ec;
	const bool is_equivalent = fs::equivalent(p1, p2, ec);

	return (is_equivalent && !ec);
}

std::wstring File::OpenFileDialog(
	const std::wstring& title,
	FILEOPENDIALOGOPTIONS options,
	LPCWSTR filter,
	HWND owner
) {
	std::wstring _Output = L"";

	CoUninitialize();
	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetOptions(options);
			pFileOpen->SetTitle(title.c_str());
			hr = pFileOpen->Show(owner);

			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr))
					{
						_Output.assign(pszFilePath);
						CoTaskMemFree(pszFilePath);
					}

					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return _Output;
}

bool File::GetAppDataPath(std::wstring& mPath)
{
	WCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		mPath.assign(szPath);
		return true;
	}

	DebugErrorL(__FUNCTION__, " -> Failed to get the path");
	return false;
}