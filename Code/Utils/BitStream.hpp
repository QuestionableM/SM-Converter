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
	const std::vector<Byte>* data;
	int index = 0;

public:
	inline BitStream(MemoryWrapper& memory)
	{
		this->data = memory.GetByteStorage();
		this->index = static_cast<int>(memory.Index() * 8);
	}

#if defined(DEBUG) || defined(_DEBUG)
	inline void DumpContentsIntoFile(const std::wstring& file_name, const std::size_t& num_bytes)
	{
		if (File::Exists(file_name))
			return;

		std::ofstream v_output(file_name, std::ios::binary);
		if (v_output.is_open())
		{
			const int v_index_copy = this->Index();

			const std::size_t v_bytes_clamped = std::min(data->size(), static_cast<std::size_t>(v_index_copy) + num_bytes);
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

	inline int Index() const noexcept
	{
		return this->index;
	}

	inline void SetIndex(int value) noexcept
	{
		this->index = value;
	}

	inline int Size() const noexcept
	{
		return static_cast<int>(this->data->size());
	}

	inline void Align() noexcept
	{
		index += (7 - ((index - 1) & 7));
	}

	inline void Move(int step) noexcept
	{
		index += step;
	}

	inline const Byte* Data() const noexcept
	{
		return this->data->data();
	}

	template<typename T>
	inline T ReadObject()
	{
		static_assert(std::is_arithmetic_v<T>, "BitStream::ReadObject -> Typename must be of arithmetic type!");

		if (Size() < (index >> 3) + sizeof(T)) return 0;

		const std::size_t v_mem_off = (index >> 3);
		const std::size_t v_offset = (index & 7);

		T v_result = 0;
		index += sizeof(T) * 8;

		const std::vector<Byte>& v_data = *this->data;

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

		bits.integer_bits = ReadObject<int>();

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
		const long long first = this->ReadObject<long long>();
		const long long second = this->ReadObject<long long>();

		return SMUuid(second, first, true);
	}

	inline std::vector<Byte> ReadBytes(int length)
	{
		std::vector<Byte> bytes(static_cast<std::size_t>(length));

		std::memcpy(bytes.data(), this->data->data() + (index >> 3), length);
		index += length * 8;

		return bytes;
	}

	inline std::string ReadString(int length)
	{
		const std::vector<Byte> bytes = ReadBytes(length);

		return std::string(bytes.begin(), bytes.end());
	}
};

SM_MANAGED_CODE