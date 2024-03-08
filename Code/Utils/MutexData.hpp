#pragma once

#include <mutex>

template<typename T>
class MutexData : public std::mutex
{
public:
	MutexData() = default;
	~MutexData() = default;

	const T& data() const { return m_data; }
	T& data() { return m_data; }

private:
	T m_data;
};