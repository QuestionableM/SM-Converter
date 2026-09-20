#pragma once

#include "UStd/UnmanagedVector.hpp"
#if defined(DEBUG) || defined(_DEBUG)
#include "UStd/UnmanagedFstream.hpp"
#include "UStd/UnmanagedString.hpp"
#include "Utils/File.hpp"
#endif

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Uuid.hpp"

#if defined(DEBUG) || defined(_DEBUG)
#define BIT_STREAM_DUMP_INTO_FILE(stream, file_name, num_bytes) stream.DumpContentsIntoFile(file_name, num_bytes)
#else
#define BIT_STREAM_DUMP_INTO_FILE(...) ((void*)0)
#endif

SM_UNMANAGED_CODE

class BitStream
{
	const std::vector<Byte>* m_data;
	std::uint32_t m_index;

public:
	inline BitStream(MemoryWrapper& memory)
		: m_data(memory.GetByteStorage())
		, m_index(static_cast<std::uint32_t>(memory.Index() * 8))
	{}

#if defined(DEBUG) || defined(_DEBUG)
	inline void DumpContentsIntoFile(const std::wstring& file_name, const std::size_t& num_bytes)
	{
		if (File::Exists(file_name))
			return;

		std::ofstream v_output(file_name, std::ios::binary);
		if (v_output.is_open())
		{
			const std::uint32_t v_index_copy = this->Index();

			const std::size_t v_bytes_clamped = std::min(m_data->size(), static_cast<std::size_t>(v_index_copy) + num_bytes);
			for (std::size_t a = 0; a < v_bytes_clamped; a++)
			{
				const unsigned char v_cur_byte = static_cast<unsigned char>(this->ReadByte());
				v_output.write(reinterpret_cast<const char*>(&v_cur_byte), 1);
			}

			v_output.close();
			this->SetIndex(v_index_copy);
		}
	}
#endif

	inline std::uint32_t Index() const noexcept
	{
		return m_index;
	}

	inline void SetIndex(std::uint32_t value) noexcept
	{
		m_index = value;
	}

	inline std::uint32_t Size() const noexcept
	{
		return static_cast<std::uint32_t>(m_data->size());
	}

	inline void Align() noexcept
	{
		m_index += (7 - ((m_index - 1) & 7));
	}

	inline void Move(const std::uint32_t step) noexcept
	{
		m_index += step;
	}

	inline const Byte* Data() const noexcept
	{
		return m_data->data();
	}

	template<typename T>
	inline T ReadObject()
	{
		static_assert(std::is_arithmetic_v<T>, "BitStream::ReadObject -> Typename must be of arithmetic type!");

		if (Size() < (m_index >> 3) + sizeof(T)) return 0;

		const std::size_t v_mem_off = (m_index >> 3);
		const std::size_t v_offset = (m_index & 7);

		T v_result = 0;
		m_index += sizeof(T) * 8;

		const std::vector<Byte>& v_data = *m_data;

		if (v_offset == 0)
		{
			for (std::size_t i = 0; i < sizeof(T); i++)
				v_result |= T(v_data[v_mem_off + i]) << ((sizeof(T) - i - 1) << 3);
		}
		else
		{
			const std::size_t v_offset_inv = (8 - v_offset);

			for (std::size_t i = 0; i < sizeof(T); i++)
			{
				const Byte v_byte_a = v_data[v_mem_off + i    ] << v_offset;
				const Byte v_byte_b = v_data[v_mem_off + i + 1] >> v_offset_inv;

				v_result |= T(v_byte_a | v_byte_b) << ((sizeof(T) - i - 1) << 3);
			}
		}

		return v_result;
	}

	inline float ReadFloat()
	{
		union
		{
			int integer_bits;
			float converted_float_bits;
		} bits = {};

		bits.integer_bits = ReadObject<std::uint32_t>();

		return bits.converted_float_bits;
	}

	inline glm::vec3 ReadVec3()
	{
		const float vec3_x = this->ReadFloat();
		const float vec3_y = this->ReadFloat();
		const float vec3_z = this->ReadFloat();

		return { vec3_x, vec3_y, vec3_z };
	}

	inline glm::quat ReadQuat()
	{
		const float quat_x = this->ReadFloat();
		const float quat_y = this->ReadFloat();
		const float quat_z = this->ReadFloat();
		const float quat_w = this->ReadFloat();

		return { quat_w, quat_x, quat_y, quat_z };
	}

	inline std::uint32_t ReadU32()
	{
		return this->ReadObject<std::uint32_t>();
	}

	inline std::uint8_t ReadU8()
	{
		return this->ReadObject<std::uint8_t>();
	}

	inline int ReadInt()
	{
		return int(this->ReadObject<int>());
	}

	inline int ReadShort()
	{
		return int(this->ReadObject<short>());
	}

	inline int ReadByte()
	{
		return int(this->ReadObject<Byte>());
	}

	inline SMUuid ReadUuid()
	{
		const std::uint64_t first = this->ReadObject<std::uint64_t>();
		const std::uint64_t second = this->ReadObject<std::uint64_t>();

		return SMUuid(second, first, true);
	}

	inline std::vector<Byte> ReadBytes(const std::uint32_t length)
	{
		std::vector<Byte> bytes(length);

		std::memcpy(bytes.data(), m_data->data() + (m_index >> 3), length);
		m_index += length * 8;

		return bytes;
	}

	inline std::string ReadString(const std::uint32_t length)
	{
		const std::vector<Byte> bytes = ReadBytes(length);

		return std::string(bytes.begin(), bytes.end());
	}
};

SM_MANAGED_CODE