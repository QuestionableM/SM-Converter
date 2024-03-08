#pragma once

#include "UStd/UnmanagedAlgorithm.hpp"
#include "UStd/UnmanagedVector.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedArray.hpp"

#if defined(DEBUG) || defined(_DEBUG)
#include "UStd/UnmanagedFstream.hpp"
#define MEMORY_WRAPPER_DUMP_BYTES(wrapper, file_name, offset) wrapper.DumpBytes(file_name, offset)
#else
#define MEMORY_WRAPPER_DUMP_BYTES(...) ((void*)0)
#endif

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"

SM_UNMANAGED_CODE

//Memory wrapper works by using the pointer to a vector
//Make sure that the vector is in the same scope as the MemoryWrapper
class MemoryWrapper
{
	const std::vector<Byte>* bytes;
	std::size_t index = 0;

public:
	MemoryWrapper() = default;
	MemoryWrapper(const MemoryWrapper&) = delete;

	inline MemoryWrapper(const std::vector<Byte>& bytes)
	{
		this->bytes = &bytes;
	}

	inline void operator=(const std::vector<Byte>& bytes)
	{
		this->bytes = &bytes;
	}

	inline MemoryWrapper& Skip(std::size_t count)
	{
		this->index += count;

		return *this;
	}

	inline MemoryWrapper& Set(std::size_t index)
	{
		this->index = index;

		return *this;
	}

	inline std::size_t Index() const noexcept
	{
		return this->index;
	}

	inline std::size_t Size() const noexcept
	{
		return this->bytes->size();
	}

	inline const Byte* Data() const noexcept
	{
		return this->bytes->data();
	}

	inline const Byte* DataPtr() const noexcept
	{
		return this->bytes->data() + this->index;
	}

	inline const std::vector<Byte>* GetByteStorage() const noexcept
	{
		return this->bytes;
	}

#if defined(DEBUG) || defined(_DEBUG)
	inline void DumpBytes(const std::string& file_name, const std::size_t& offset = 0)
	{
		std::ofstream v_output(file_name, std::ios::binary);
		if (!v_output.is_open())
			return;
		
		const std::size_t v_diff = std::min<std::size_t>(this->bytes->size(), offset);
		const std::size_t v_size = this->bytes->size() - v_diff;
		for (std::size_t a = offset; a < v_size; a++)
			v_output.write(reinterpret_cast<const char*>(&this->bytes[a]), 1);

		v_output.close();
	}
#endif

	template<typename T, bool is_big_endian = false>
	inline T Object(std::size_t offset)
	{
		constexpr const std::size_t type_sz = sizeof(T);

		if constexpr (type_sz > 1) //minor optimization for 1 byte data
		{
			Byte obj_bytes[type_sz];
			std::memcpy(obj_bytes, this->bytes->data() + offset, type_sz);

			if constexpr (is_big_endian)
			{
				std::reverse(obj_bytes, obj_bytes + type_sz);
			}

			return *reinterpret_cast<T*>(obj_bytes);
		}
		else
		{
			return *reinterpret_cast<const T*>(&(*this->bytes)[offset]);
		}
	}

	template<typename T, bool is_big_endian = false>
	inline T NextObject()
	{
		constexpr std::size_t type_sz = sizeof(T);

		Byte obj_bytes[type_sz];
		std::memcpy(obj_bytes, this->bytes->data() + this->index, type_sz);
		this->index += type_sz;

		if constexpr (is_big_endian)
		{
			std::reverse(obj_bytes, obj_bytes + type_sz);
		}

		return *reinterpret_cast<T*>(obj_bytes);
	}

	template<typename T, bool is_big_endian = false>
	inline void NextObjectRef(T* object)
	{
		std::memcpy(object, this->bytes->data() + this->index, sizeof(T));
		this->index += sizeof(T);

		if constexpr (is_big_endian)
		{
			Byte* v_obj_start = &object;
			Byte* v_obj_end = v_obj_start + sizeof(T);

			std::reverse(v_obj_start, v_obj_end);
		}
	}

	template<typename T, std::size_t array_size, bool is_big_endian = false>
	inline std::array<T, array_size> ObjectsConst(std::size_t offset)
	{
		std::array<T, array_size> l_output;
		std::memcpy(l_output.data(), this->bytes->data() + offset, sizeof(T) * array_size);

		if constexpr (is_big_endian)
		{
			std::reverse(l_output.begin(), l_output.end());
		}

		return l_output;
	}

	glm::quat GetQuat(std::size_t offset)
	{
		const std::array<float, 4> f_quat = this->ObjectsConst<float, 4>(offset);

		return { f_quat[3], f_quat[0], f_quat[1], f_quat[2] };
	}

	template<bool is_big_endian = false>
	inline std::string String(std::size_t offset, std::size_t size)
	{
		std::string v_string;
		v_string.resize(size);

		std::memcpy(v_string.data(), this->bytes->data() + offset, sizeof(char) * size);

		if constexpr (is_big_endian)
		{
			std::reverse(v_string.begin(), v_string.end());
		}

		return v_string;
	}

	template<bool is_big_endian = false>
	inline std::string NextString(std::size_t size)
	{
		std::string v_string;
		v_string.resize(size);

		std::memcpy(v_string.data(), this->bytes->data() + this->index, sizeof(char) * size);
		this->index += size;

		if constexpr (is_big_endian)
		{
			std::reverse(v_string.begin(), v_string.end());
		}

		return v_string;
	}

	template<typename T, bool is_big_endian = false>
	inline std::vector<T> Objects(std::size_t offset, std::size_t amount)
	{
		std::vector<T> obj_copy(amount);

		std::memcpy(obj_copy.data(), this->bytes->data() + offset, sizeof(T) * amount);

		if constexpr (is_big_endian)
		{
			std::reverse(obj_copy.begin(), obj_copy.end());
		}

		return obj_copy;
	}

	template<typename T, bool is_big_endian = false>
	inline void ObjectsRef(T* object_array, std::size_t offset, std::size_t amount)
	{
		const std::size_t final_amount = sizeof(T) * amount;
		std::memcpy(object_array, this->bytes->data() + offset, final_amount);

		if constexpr (is_big_endian)
		{
			std::reverse(object_array, object_array + final_amount);
		}
	}

	template<typename T, bool is_big_endian = false>
	inline std::vector<T> NextObjects(std::size_t amount)
	{
		const std::size_t byte_amount = sizeof(T) * amount;

		std::vector<T> obj_copy(amount);

		std::memcpy(obj_copy.data(), this->bytes->data() + this->index, byte_amount);
		this->index += byte_amount;

		if constexpr (is_big_endian)
		{
			std::reverse(obj_copy.begin(), obj_copy.end());
		}

		return obj_copy;
	}

	template<typename T, bool is_big_endian = false>
	inline void NextObjectsRef(T* object_array, std::size_t amount)
	{
		const std::size_t byte_amount = sizeof(T) * amount;

		std::memcpy(object_array, this->bytes->data() + this->index, byte_amount);
		this->index += byte_amount;

		if constexpr (is_big_endian)
		{
			std::reverse(object_array, object_array + byte_amount);
		}
	}
};

SM_MANAGED_CODE