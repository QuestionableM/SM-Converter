#include "Color.hpp"

SM_UNMANAGED_CODE

SMColor::SMColor() noexcept :
	m_color(0x000000ff)
{}

SMColor::SMColor(const std::string_view& color) noexcept
{
	this->fromStringView(color);
}

SMColor::SMColor(Byte r, Byte g, Byte b) noexcept :
	m_bytes{ r, g, b, 255 }
{}

SMColor::SMColor(Byte r_val, Byte g_val, Byte b_val, Byte a_val) noexcept :
	r(r_val),
	g(g_val),
	b(b_val),
	a(a_val)
{}

SMColor::SMColor(std::uint32_t color) noexcept
{
	this->SetIntLittleEndian(color);
}

void SMColor::operator=(const std::string_view& color) noexcept
{
	this->fromStringView(color);
}

void SMColor::SetIntLittleEndian(std::uint32_t color) noexcept
{
	this->r = static_cast<Byte>(color);
	this->g = static_cast<Byte>(color >> 8);
	this->b = static_cast<Byte>(color >> 16);
	this->a = static_cast<Byte>(color >> 24);
}

void SMColor::SetIntBigEndian(std::uint32_t color) noexcept
{
	this->r = static_cast<Byte>(color >> 24);
	this->g = static_cast<Byte>(color >> 16);
	this->b = static_cast<Byte>(color >> 8);
	this->a = static_cast<Byte>(color);
}

std::string SMColor::String() const
{
	return std::to_string(this->r) + " " + std::to_string(this->g) + " " + std::to_string(this->b);
}

std::string SMColor::StringNormalized() const
{
	const float v_norm_r = static_cast<float>(this->r) / 255.0f;
	const float v_norm_g = static_cast<float>(this->g) / 255.0f;
	const float v_norm_b = static_cast<float>(this->b) / 255.0f;

	return std::to_string(v_norm_r) + " " + std::to_string(v_norm_g) + " " + std::to_string(v_norm_b);
}

std::string SMColor::StringHex() const
{
	char v_buffer[6];
	this->StringHexCStr(v_buffer);

	return std::string(v_buffer, 6);
}

std::string SMColor::StringHexAlpha() const
{
	char v_buffer[8];
	String::ByteToHexString(v_buffer, this->r);
	String::ByteToHexString(v_buffer + 2, this->g);
	String::ByteToHexString(v_buffer + 4, this->b);
	String::ByteToHexString(v_buffer + 6, this->a);

	return std::string(v_buffer, 8);
}

char* SMColor::StringHexCStr(char* ptr) const noexcept
{
	String::ByteToHexString(ptr, this->r);
	String::ByteToHexString(ptr + 2, this->g);
	String::ByteToHexString(ptr + 4, this->b);
	return ptr + 6;
}

void SMColor::appendStringHex(std::string& outStr) const
{
	char v_buffer[6];
	this->StringHexCStr(v_buffer);

	outStr.append(v_buffer, 6);
}

char* SMColor::WriteEmptyHex(char* ptr) noexcept
{
	*ptr++ = '0';
	*ptr++ = '0';
	*ptr++ = '0';
	*ptr++ = '0';
	*ptr++ = '0';
	*ptr++ = '0';
	return ptr;
}

void SMColor::fromStringView(const std::string_view& color) noexcept
{
	if (color.empty())
	{
		m_color = 0x000000ff;
		return;
	}

	const std::size_t v_ptrAdder(color[0] == '#');
	const std::size_t v_lessCheck = 6 + v_ptrAdder;
	const std::size_t v_moreCheck = 8 + v_ptrAdder;

	if (color.size() < v_lessCheck)
	{
		m_color = 0x000000ff;
		return;
	}

	char v_strCpy[9] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 'f', 'f', 0x0 };
	std::memcpy(v_strCpy, color.data() + v_ptrAdder, (color.size() >= v_moreCheck) ? 8 : 6);

	this->r = String::HexStringToByte(v_strCpy);
	this->g = String::HexStringToByte(v_strCpy + 2);
	this->b = String::HexStringToByte(v_strCpy + 4);
	this->a = String::HexStringToByte(v_strCpy + 6);
}