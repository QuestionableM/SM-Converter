#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class ConvertError
{
public:
	inline ConvertError() :
		m_errorCode(0),
		m_errorMessage()
	{}

	inline ConvertError(unsigned short ec, const std::wstring& error_msg) :
		m_errorCode(ec),
		m_errorMessage(error_msg)
	{}

	inline ConvertError(const std::wstring& error_msg, const std::wstring& func_string) :
		m_errorCode(1),
		m_errorMessage(func_string + L" -> " + error_msg)
	{}

	inline explicit operator bool() const noexcept
	{
		return (m_errorCode != 0);
	}

	inline const std::wstring& getErrorMsg() const noexcept
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