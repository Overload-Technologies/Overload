/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <charconv>
#include <format>

#include <OvPhysics/Settings/CollisionLayers.h>

namespace
{
	constexpr uint32_t kFreeSlotMask = ~0u;

	std::string GetLayerKey(uint32_t p_layer)
	{
		return "collision_layer_" + std::to_string(p_layer);
	}

	std::string GetMaskKey(uint32_t p_layer)
	{
		return "collision_mask_" + std::to_string(p_layer);
	}

	std::optional<uint32_t> ParseMask(const std::string& p_value)
	{
		uint32_t mask = 0;
		const auto end = p_value.data() + p_value.size();
		const auto result = std::from_chars(p_value.data(), end, mask, 16);

		if (result.ec != std::errc{} || result.ptr != end)
		{
			return {};
		}

		return mask;
	}

	void StoreSetting(OvTools::Filesystem::IniFile& p_settings, const std::string& p_key, const std::string& p_value)
	{
		if (p_settings.IsKeyExisting(p_key))
		{
			p_settings.Set<std::string>(p_key, p_value);
		}
		else
		{
			p_settings.Add<std::string>(p_key, p_value);
		}
	}
}

OvPhysics::Settings::CollisionLayers::CollisionLayers()
{
	m_names[kDefaultLayer] = kDefaultLayerName;
	m_masks.fill(kFreeSlotMask);
}

std::optional<uint32_t> OvPhysics::Settings::CollisionLayers::AddLayer(const std::string& p_name)
{
	if (!IsNameValid(p_name) || FindLayer(p_name))
	{
		return {};
	}

	for (uint32_t layer = 0; layer < kMaxLayerCount; ++layer)
	{
		if (!IsLayerUsed(layer))
		{
			m_names[layer] = p_name;
			return layer;
		}
	}

	return {};
}

void OvPhysics::Settings::CollisionLayers::RemoveLayer(uint32_t p_layer)
{
	if (p_layer >= kMaxLayerCount || p_layer == kDefaultLayer)
	{
		return;
	}

	m_names[p_layer].clear();

	/* A free slot collides with everything, so the next layer taking it starts from a clean matrix */
	for (auto& mask : m_masks)
	{
		mask |= 1u << p_layer;
	}

	m_masks[p_layer] = kFreeSlotMask;
}

void OvPhysics::Settings::CollisionLayers::RenameLayer(uint32_t p_layer, const std::string& p_name)
{
	if (p_layer >= kMaxLayerCount || p_layer == kDefaultLayer || !IsNameValid(p_name) || FindLayer(p_name))
	{
		return;
	}

	m_names[p_layer] = p_name;
}

bool OvPhysics::Settings::CollisionLayers::IsLayerUsed(uint32_t p_layer) const
{
	return p_layer < kMaxLayerCount && !m_names[p_layer].empty();
}

const std::string& OvPhysics::Settings::CollisionLayers::GetLayerName(uint32_t p_layer) const
{
	return m_names[p_layer < kMaxLayerCount ? p_layer : kDefaultLayer];
}

std::optional<uint32_t> OvPhysics::Settings::CollisionLayers::FindLayer(std::string_view p_name) const
{
	if (p_name.empty())
	{
		return {};
	}

	for (uint32_t layer = 0; layer < kMaxLayerCount; ++layer)
	{
		if (m_names[layer] == p_name)
		{
			return layer;
		}
	}

	return {};
}

void OvPhysics::Settings::CollisionLayers::SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide)
{
	if (p_first >= kMaxLayerCount || p_second >= kMaxLayerCount)
	{
		return;
	}

	if (p_collide)
	{
		m_masks[p_first] |= 1u << p_second;
		m_masks[p_second] |= 1u << p_first;
	}
	else
	{
		m_masks[p_first] &= ~(1u << p_second);
		m_masks[p_second] &= ~(1u << p_first);
	}
}

bool OvPhysics::Settings::CollisionLayers::GetLayerCollision(uint32_t p_first, uint32_t p_second) const
{
	if (p_first >= kMaxLayerCount || p_second >= kMaxLayerCount)
	{
		return false;
	}

	return (m_masks[p_first] & (1u << p_second)) != 0;
}

uint32_t OvPhysics::Settings::CollisionLayers::GetLayerMask(uint32_t p_layer) const
{
	return m_masks[p_layer < kMaxLayerCount ? p_layer : kDefaultLayer];
}

void OvPhysics::Settings::CollisionLayers::Deserialize(const OvTools::Filesystem::IniFile& p_settings)
{
	*this = CollisionLayers{};

	for (uint32_t layer = 0; layer < kMaxLayerCount; ++layer)
	{
		const auto name = p_settings.GetOrDefault<std::string>(GetLayerKey(layer), {});

		if (layer != kDefaultLayer && IsNameValid(name) && !FindLayer(name))
		{
			m_names[layer] = name;
		}

		if (const auto mask = ParseMask(p_settings.GetOrDefault<std::string>(GetMaskKey(layer), {})))
		{
			m_masks[layer] = mask.value();
		}
	}

	/* The matrix has to stay symmetric, even when the project file was edited by hand */
	for (uint32_t first = 0; first < kMaxLayerCount; ++first)
	{
		for (uint32_t second = first + 1; second < kMaxLayerCount; ++second)
		{
			SetLayerCollision(first, second, GetLayerCollision(first, second) && GetLayerCollision(second, first));
		}
	}
}

void OvPhysics::Settings::CollisionLayers::Serialize(OvTools::Filesystem::IniFile& p_settings) const
{
	for (uint32_t layer = 0; layer < kMaxLayerCount; ++layer)
	{
		if (IsLayerUsed(layer))
		{
			StoreSetting(p_settings, GetLayerKey(layer), m_names[layer]);
			StoreSetting(p_settings, GetMaskKey(layer), std::format("{:08X}", m_masks[layer]));
		}
		else
		{
			p_settings.Remove(GetLayerKey(layer));
			p_settings.Remove(GetMaskKey(layer));
		}
	}
}

bool OvPhysics::Settings::CollisionLayers::IsNameValid(std::string_view p_name) const
{
	/* Equal signs would break the serialized form, and surrounding spaces wouldn't survive a round
	   trip through the project file */
	constexpr std::string_view kSpaces = " \t";

	return
		!p_name.empty() &&
		p_name.find('=') == std::string_view::npos &&
		kSpaces.find(p_name.front()) == std::string_view::npos &&
		kSpaces.find(p_name.back()) == std::string_view::npos;
}
