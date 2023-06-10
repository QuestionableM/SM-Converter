#pragma once

#include "UStd\UnmanagedVector.hpp"
#if defined(DEBUG) || defined(_DEBUG)
#include "UStd\UnmanagedFstream.hpp"
#include "UStd\UnmanagedString.hpp"
#include "Utils\File.hpp"
#endif

#include "Utils\ByteImpl.hpp"
#include "Utils\Memory.hpp"
#include "Utils\Uuid.hpp"

#if defined(DEBUG) || defined(_DEBUG)
#define BIT_STREAM_DUMP_INTO_FILE(stream, file_name, num_bytes) stream.DumpContentsIntoFile(file_name, num_bytes)
#else
#define BIT_STREAM_DUMP_INTO_FILE(...) ((void*)0)
#endif

#pragma unmanaged

class BitStream
{
	std::vector<Byte> data;
	int index = 0;

public:
	inline BitStream(MemoryWrapper& memory)
	{
		const std::size_t chunk_size = memory.Size() - memory.Index();
		data.resize(chunk_size);

		std::memcpy(data.data(), memory.Data() + memory.Index(), chunk_size);
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

			const std::size_t v_bytes_clamped = std::min(data.size(), static_cast<std::size_t>(v_index_copy) + num_bytes);
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
		return static_cast<int>(this->data.size());
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
		return this->data.data();
	}

	inline long long ReadNBytes(int count)
	{
		if (Size() < (index >> 3) + count) return 0;

		long long mem_off = (index >> 3);
		int offset = (index & 7);

		long long result = 0;
		index += count * 8;

		if (offset == 0)
		{
			for (long long i = 0; i < count; i++)
			{
				const std::size_t mem_off_full = (std::size_t)(mem_off + i);

				result |= (((long long)data[mem_off_full]) & 0xff) << ((count - i - 1ll) << 3ll);
			}

			return result;
		}

		for (long long i = 0; i < count; i++)
		{
			const std::size_t mem_off_full = (std::size_t)(mem_off + i);

			long a = ((((long)data[mem_off_full    ] & 0xff) << (    offset))) & 0xff;
			long b = ((((long)data[mem_off_full + 1] & 0xff) >> (8 - offset))) & 0xff;

			long long c = (a | b) & 0xff;

			result |= (c << ((count - i - 1ll) << 3ll));
		}

		return result;
	}

	union int_to_float_bits
	{
		int integer_bits;
		float converted_float_bits;
	};

	inline float ReadFloat()
	{
		union int_to_float_bits bits = {};
		bits.integer_bits = (int)ReadNBytes(4);

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
		return (int)ReadNBytes(4);
	}

	inline int ReadShort()
	{
		return (int)ReadNBytes(2);
	}

	inline int ReadByte()
	{
		return (int)ReadNBytes(1);
	}

	inline SMUuid ReadUuid()
	{
		const long long first = ReadNBytes(8);
		const long long second = ReadNBytes(8);

		return SMUuid(second, first, true);
	}

	inline std::vector<Byte> ReadBytes(int length)
	{
		std::vector<Byte> bytes(static_cast<std::size_t>(length));

		std::memcpy(bytes.data(), data.data() + (index >> 3), length);
		index += length * 8;

		return bytes;
	}

	inline std::string ReadString(int length)
	{
		const std::vector<Byte> bytes = ReadBytes(length);

		return std::string(bytes.begin(), bytes.end());
	}
};

#pragma managed