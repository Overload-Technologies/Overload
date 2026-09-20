/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <utility>

#include <OvCore/ResourceManagement/UIResourceRegistry.h>

void OvCore::ResourceManagement::UIResourceRegistry::ProvideDefinition(Definition p_definition)
{
	m_definition = std::move(p_definition);
}

const OvCore::ResourceManagement::UIResourceRegistry::Definition& OvCore::ResourceManagement::UIResourceRegistry::GetDefinition() const
{
	return m_definition;
}
