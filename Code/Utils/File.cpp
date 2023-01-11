#include "File.hpp"

#include "Console.hpp"

#include <Windows.h>
#include <ShlObj.h>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

std::vector<Byte> File::ReadFileBytes(const std::wstring& path)
{
	std::ifstream input_file(path, std::ios::binary);
	std::vector<Byte> file_bytes = {};

	if (input_file.is_open())
	{
		input_file.seekg(0, std::ios::end);
		const std::size_t file_size = static_cast<std::size_t>(input_file.tellg());
		input_file.seekg(0, std::ios::beg);

		file_bytes.resize(file_size);
		input_file.read(reinterpret_cast<char*>(file_bytes.data()), file_size);

		input_file.close();
	}

	return file_bytes;
}

bool File::ReadToString(const std::wstring& path, std::string& r_output)
{
	std::ifstream input_file(path, std::ios::binary);
	if (!input_file.is_open()) return false;

	input_file.seekg(0, std::ios::end);
	r_output.resize(input_file.tellg());
	input_file.seekg(0, std::ios::beg);

	input_file.read(r_output.data(), r_output.size());
	input_file.close();

	return true;
}

bool File::ReadToStringNormal(const std::wstring& path, std::string& r_output)
{
	std::ifstream v_input_file(path);
	if (!v_input_file.is_open()) return false;

	v_input_file.seekg(0, std::ios::end);
	r_output.resize(v_input_file.tellg());
	v_input_file.seekg(0, std::ios::beg);

	v_input_file.read(r_output.data(), r_output.size());
	v_input_file.close();

	return true;
}

bool File::Exists(const std::wstring& path)
{
	std::error_code ec;
	bool exists = fs::exists(path, ec);

	return (!ec && exists);
}

bool File::CreateDirectorySafe(const std::wstring& path)
{
	std::error_code e_error;
	bool exists = fs::exists(path, e_error);
	bool exists_correct = (!e_error && exists);

	std::error_code f_error;
	bool file_created = fs::create_directory(path, f_error);
	bool file_correct = (!f_error && file_created);

	return (exists_correct || file_correct);
}

bool File::Equivalent(const std::wstring& p1, const std::wstring& p2)
{
	std::error_code ec;
	bool is_equivalent = fs::equivalent(p1, p2, ec);

	return (is_equivalent && !ec);
}

std::wstring File::OpenFileDialog(
	const std::wstring& title,
	FILEOPENDIALOGOPTIONS options,
	LPCWSTR filter,
	HWND owner
) {
	std::wstring _Output = L"";

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
						_Output = std::wstring(pszFilePath);
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
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		mPath = std::wstring(szPath);

		return true;
	}

	DebugErrorL(__FUNCTION__, " -> Failed to get the path");
	return false;
}