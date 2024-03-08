#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMTileClutter : public SMEntityWithModel
{
public:
	inline SMTileClutter(ClutterData* pParent, Model* pModel)
		: SMEntityWithModel(pModel), m_parent(pParent) {}

	SMTileClutter(SMTileClutter&) = delete;
	SMTileClutter(const SMTileClutter&) = delete;
	~SMTileClutter() = default;

	const SMUuid& GetUuid() const override { return m_parent->m_uuid; }

	inline float ScaleVariance() const noexcept { return m_parent->m_scaleVariance; }

	inline void SetColor(SMColor color) { this->m_color = color; }

	inline EntityType Type() const noexcept override { return EntityType::Clutter; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	std::string GetMtlName(std::size_t mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;

private:
	class ClutterData* m_parent;
	SMColor m_color = 0xffffff;
};

SM_MANAGED_CODE