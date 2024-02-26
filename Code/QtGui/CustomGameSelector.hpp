#pragma once

#include "AlignedGroupBox.hpp"

#include <string>

class CustomGameSelector : public NamedComboBox
{
public:
	CustomGameSelector(QWidget* parent, const std::wstring& path);
	~CustomGameSelector() = default;

	int getCustomGameIndex() const;
};