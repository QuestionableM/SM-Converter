#include "UserCharacterData.hpp"

#include "UStd\UnmanagedFstream.hpp"

#pragma unmanaged

template<typename T>
inline void read_bytes_r(std::ifstream& v_file, T& v_value)
{
	char v_buffer[sizeof(T)];

	v_file.read(v_buffer, sizeof(T));
	std::reverse(v_buffer, v_buffer + sizeof(T));

	v_value = *reinterpret_cast<T*>(v_buffer);
}

bool UserCharacterData::from_file(const std::wstring& path)
{
	std::ifstream v_input(path);
	if (!v_input.is_open())
		return false;

	v_input.seekg(0, std::ios::end);
	const std::size_t v_input_sz = static_cast<std::size_t>(v_input.tellg());
	v_input.seekg(0, std::ios::beg);

	std::size_t v_size_check = 6;
	if (v_input_sz < v_size_check)
		return false;

	read_bytes_r(v_input, this->version);
	if (this->version != 2)
	{
		DebugErrorL("Unsupported character file version: ", this->version);
		return false;
	}

	v_input.read(reinterpret_cast<char*>(&this->is_female), sizeof(this->is_female));

	unsigned char v_num_garments = 0;
	v_input.read(reinterpret_cast<char*>(&v_num_garments), sizeof(v_num_garments));

	if (v_num_garments != 9)
		return false;

	std::size_t v_num_garments_sz(v_num_garments);
	v_size_check += (v_num_garments_sz * 16) + (v_num_garments_sz * 4);
	if (v_input_sz != v_size_check)
	{
		DebugErrorL("The amount of bytes doesn't match with reference value: ", v_input_sz, " != ", v_size_check);
		return false;
	}

	for (std::size_t a = 0; a < v_num_garments_sz; a++)
		v_input.read(reinterpret_cast<char*>(&this->garments[a]), sizeof(SMUuid));

	for (std::size_t a = 0; a < v_num_garments_sz; a++)
		read_bytes_r(v_input, this->colors[a]);

	return true;
}