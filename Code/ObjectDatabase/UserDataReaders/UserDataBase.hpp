#pragma once

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <string>
#include <vector>
#include <atomic>

template<typename T, typename TObjectType>
class UserDataBase
{
public:
	using ObjectType = TObjectType;

	inline static std::vector<TObjectType>& GetCurrentStorage()
	{
		return T::ShouldUseFilteredStorage() ? T::FilteredStorage : T::Storage;
	}

	inline constexpr static void UpdateStorage()
	{
		if (T::ShouldUseFilteredStorage())
			T::FilterStorage();
	}

	inline static void Search(
		const std::wstring& search_str,
		int last_search_length,
		int cur_search_length)
	{
		if (T::Storage.empty())
			return;

		if (last_search_length != 0 && cur_search_length > last_search_length)
		{
			std::size_t v_newCacheSize = 0;

			for (TObjectType v_cur_instance : T::SearchResults)
				if (v_cur_instance->lower_name.find(search_str) != std::wstring::npos)
					T::SearchResults[v_newCacheSize++] = v_cur_instance;

			T::SearchResults.resize(v_newCacheSize);
		}
		else
		{
			T::SearchResults.clear();

			const std::vector<TObjectType>& v_cur_array = T::GetCurrentStorage();
			for (TObjectType v_cur_instance : v_cur_array)
				if (v_cur_instance->lower_name.find(search_str) != std::wstring::npos)
					T::SearchResults.push_back(v_cur_instance);
		}
	}

	inline static void PushToStorage(const TObjectType& new_object)
	{
		UserDataBase<T, TObjectType>::ObjectCounter++;
		UserDataBase<T, TObjectType>::Storage.push_back(new_object);
	}

	inline static void ClearBase()
	{
		UserDataBase<T, TObjectType>::FilteredStorage.clear();
		UserDataBase<T, TObjectType>::SearchResults.clear();
		UserDataBase<T, TObjectType>::Storage.clear();

		UserDataBase<T, TObjectType>::ObjectCounter = 0;
	}

	inline static std::atomic_size_t ObjectCounter = 0;
	inline static std::vector<TObjectType> Storage = {};
	inline static std::vector<TObjectType> FilteredStorage = {};
	inline static std::vector<TObjectType> SearchResults = {};
};

SM_MANAGED_CODE