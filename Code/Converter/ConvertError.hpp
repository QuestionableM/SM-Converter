#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class ConvertError
{
public:
	ConvertError()
	{
		this->clear();
	}

	inline ConvertError(unsigned short ec, const std::wstring& error_msg)
	{
		this->m_errorCode = ec;
		this->m_errorMessage = error_msg;
	}

	inline ConvertError(const std::wstring& error_msg, const std::wstring& func_string)
	{
		this->m_errorCode = 1;
		this->m_errorMessage = func_string + L" -> " + error_msg;
	}

	inline explicit operator bool() const noexcept
	{
		return (m_errorCode != 0);
	}

	inline const std::wstring& GetErrorMsg() const noexcept
	{
		return m_errorMessage;
	}

	inline void clear()
	{
		m_errorCode = 0;
		m_errorMessage.clear();
	}

private:
	unsigned short m_errorCode = 0;
	std::wstring m_errorMessage;
};

SM_MANAGED_CODE