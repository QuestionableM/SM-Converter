#pragma once

#include "Utils\ByteImpl.hpp"

#include <algorithm>
#include <vector>
#include <array>

#include <gtc\quaternion.hpp>

class MemoryWrapper
{
	std::vector<Byte> bytes;
	std::size_t index = 0;

public:
	MemoryWrapper() = default;
	MemoryWrapper(const MemoryWrapper&) = delete;

	inline MemoryWrapper(const std::vector<Byte>& bytes)
	{
		this->bytes = bytes;
	}

	inline void operator=(const std::vector<Byte>& bytes)
	{
		this->bytes = bytes;
	}

	inline MemoryWrapper& Skip(const std::size_t& count)
	{
		this->index += count;

		return *this;
	}

	inline MemoryWrapper& Set(const std::size_t& index)
	{
		this->index = index;

		return *this;
	}

	inline std::size_t Index() const
	{
		return this->index;
	}

	inline std::size_t Size() const
	{
		return this->bytes.size();
	}

	inline const Byte* Data() const
	{
		return this->bytes.data();
	}

	template<typename T, bool is_big_endian = false>
	inline T Object(const std::size_t& offset)
	{
		constexpr const std::size_t type_sz = sizeof(T);

		Byte obj_bytes[type_sz];
		std::memcpy(obj_bytes, this->bytes.data() + offset, type_sz);

		if constexpr (is_big_endian)
		{
			std::reverse(obj_bytes, obj_bytes + type_sz);
		}

		return *reinterpret_cast<T*>(obj_bytes);
	}

	template<typename T, bool is_big_endian = false>
	inline T NextObject()
	{
		constexpr std::size_t type_sz = sizeof(T);

		Byte obj_bytes[type_sz];
		std::memcpy(obj_bytes, this->bytes.data() + this->index, type_sz);
		this->index += type_sz;

		if constexpr (is_big_endian)
		{
			std::reverse(obj_bytes, obj_bytes + type_sz);
		}

		return *reinterpret_cast<T*>(obj_bytes);
	}

	template<typename T, std::size_t array_size, bool is_big_endian = false>
	inline std::array<T, array_size> ObjectsConst(const std::size_t& offset)
	{
		std::array<T, array_size> l_output;
		std::memcpy(l_output.data(), this->bytes.data() + offset, sizeof(T) * array_size);

		if constexpr (is_big_endian)
		{
			std::reverse(l_output.begin(), l_output.end());
		}

		return l_output;
	}

	glm::quat GetQuat(const std::size_t& offset)
	{
		const std::array<float, 4> f_quat = this->ObjectsConst<float, 4>(offset);

		return { f_quat[3], f_quat[0], f_quat[1], f_quat[2] };
	}

	template<typename T, bool is_big_endian = false>
	inline std::vector<T> Objects(const std::size_t& offset, const std::size_t& amount)
	{
		std::vector<T> obj_copy;
		obj_copy.resize(amount);

		std::memcpy(obj_copy.data(), this->bytes.data() + offset, sizeof(T) * amount);

		if constexpr (is_big_endian)
		{
			std::reverse(obj_copy.begin(), obj_copy.end());
		}

		return obj_copy;
	}

	template<typename T, bool is_big_endian = false>
	inline void ObjectsRef(T* object_array, const std::size_t& offset, const std::size_t& amount)
	{
		const std::size_t final_amount = sizeof(T) * amount;
		std::memcpy(object_array, this->bytes.data() + offset, final_amount);

		if constexpr (is_big_endian)
		{
			std::reverse(object_array, object_array + final_amount);
		}
	}

	template<typename T, bool is_big_endian = false>
	inline std::vector<T> NextObjects(const std::size_t& amount)
	{
		const std::size_t byte_amount = sizeof(T) * amount;

		std::vector<T> obj_copy;
		obj_copy.resize(amount);

		std::memcpy(obj_copy.data(), this->bytes.data() + this->index, byte_amount);
		this->index += byte_amount;

		if constexpr (is_big_endian)
		{
			std::reverse(obj_copy.begin(), obj_copy.end());
		}

		return obj_copy;
	}

	template<typename T, bool is_big_endian = false>
	inline void NextObjectsRef(T* object_array, const std::size_t& amount)
	{
		const std::size_t byte_amount = sizeof(T) * amount;

		std::memcpy(object_array, this->bytes.data() + this->index, byte_amount);
		this->index += byte_amount;

		if constexpr (is_big_endian)
		{
			std::reverse(object_array, object_array + byte_amount);
		}
	}
};