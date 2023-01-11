#pragma once

#include <string>

class ConvertError
{
public:
	ConvertError() = default;

	inline ConvertError(const unsigned short& ec, const std::wstring& error_msg)
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

private:
	unsigned short m_errorCode = 0;
	std::wstring m_errorMessage;
};