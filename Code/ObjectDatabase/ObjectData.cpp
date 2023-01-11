#include "ObjectData.hpp"

std::wstring& TextureList::GetStringRef(const std::size_t& idx)
{
	return ((std::wstring*)&this->dif)[idx];
}

void TextureData::AddEntry(const std::wstring& name, const TextureList& tex_list)
{
	if (MaterialMap.find(name) != MaterialMap.end())
		return;

	MaterialMap.insert(std::make_pair(name, tex_list));
}

bool TextureData::GetEntry(const std::wstring& name, TextureList& list_ref) const
{
	if (MaterialMap.find(name) == MaterialMap.end())
		return false;

	list_ref = MaterialMap.at(name);
	return true;
}

TextureDataType TextureData::Type() const
{
	return this->type;
}

TextureData::TextureData(const TextureDataType& type)
{
	this->type = type;
}