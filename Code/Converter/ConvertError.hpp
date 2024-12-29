#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

#include <type_traits>
#include <cstdint>

class ConvertError
{
public:
	inline ConvertError() :
		m_errorCode(0),
		m_errorMessage()
	{}

	inline ConvertError(std::uint16_t ec, const std::string& error_msg) :
		m_errorCode(ec),
		m_errorMessage(error_msg)
	{}

	void operator=(const ConvertError& other) = delete;

private:
	template<std::size_t N>
	inline void setErrorVariadic(const char (&arg_str)[N])
	{
		m_errorMessage.append(arg_str, arg_str + (N - 1));
	}

	template<typename CurArg> requires(
		std::is_same_v<std::remove_cv_t<CurArg>, bool> ||
		std::is_same_v<std::remove_cv_t<CurArg>, unsigned char> ||
		std::is_same_v<std::remove_cv_t<CurArg>, short> ||
		std::is_same_v<std::remove_cv_t<CurArg>, unsigned short> ||
		std::is_same_v<std::remove_cv_t<CurArg>, int> ||
		std::is_same_v<std::remove_cv_t<CurArg>, unsigned int> ||
		std::is_same_v<std::remove_cv_t<CurArg>, long> ||
		std::is_same_v<std::remove_cv_t<CurArg>, unsigned long> ||
		std::is_same_v<std::remove_cv_t<CurArg>, long long> ||
		std::is_same_v<std::remove_cv_t<CurArg>, unsigned long long>
	)
	inline void setErrorVariadic(CurArg arg)
	{
		char v_intBuffer[64];
		const std::string_view v_result = String::IntegerToString<CurArg>(v_intBuffer, sizeof(v_intBuffer), arg);

		m_errorMessage.assign(v_result);
	}

	template<typename CurArg> requires(std::is_same_v<CurArg, char>)
	inline void setErrorVariadic(CurArg arg)
	{
		m_errorMessage.append(1, arg);
	}

	template<typename CurArg, typename ...Args>
	inline void setErrorVariadic(CurArg&& curArg, Args&& ...otherArgs)
	{
		this->setErrorVariadic(curArg);
		this->setErrorVariadic(otherArgs...);
	}

public:
	template<typename ...Args>
	inline void setError(const std::uint16_t ec, Args&& ...args)
	{
		m_errorCode = ec;
		this->setErrorVariadic(args...);
	}

	inline explicit operator bool() const noexcept
	{
		return m_errorCode != 0;
	}

	inline const std::string& getErrorMsg() const noexcept
	{
		return m_errorMessage;
	}

	inline void clear()
	{
		m_errorCode = 0;
		m_errorMessage.clear();
	}

private:
	std::uint16_t m_errorCode;
	std::string m_errorMessage;
};

SM_MANAGED_CODE