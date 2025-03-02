/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>

#include <OvCore/Global/ServiceLocator.h>
#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>
#include <OvEditor/Panels/TextureDebugger.h>
#include <OvRendering/HAL/Texture.h>
#include <OvTools/Utils/EnumMapper.h>

template <>
struct OvTools::Utils::MappingFor<OvEditor::Panels::EScaleMode, std::string>
{
	static constexpr char one_to_one[] = "1:1";
	static constexpr char percentage_5[] = "5%";
	static constexpr char percentage_10[] = "10%";
	static constexpr char percentage_25[] = "25%";
	static constexpr char percentage_50[] = "50%";
	static constexpr char percentage_100[] = "100%";
	static constexpr char percentage_200[] = "200%";
	static constexpr char percentage_400[] = "400%";
	static constexpr char percentage_800[] = "800%";

	using enum OvEditor::Panels::EScaleMode;
	using type = std::tuple<
		EnumValuePair<ONE_TO_ONE, one_to_one>,
		EnumValuePair<PERCENTAGE_5, percentage_5>,
		EnumValuePair<PERCENTAGE_10, percentage_10>,
		EnumValuePair<PERCENTAGE_25, percentage_25>,
		EnumValuePair<PERCENTAGE_50, percentage_50>,
		EnumValuePair<PERCENTAGE_100, percentage_100>,
		EnumValuePair<PERCENTAGE_200, percentage_200>,
		EnumValuePair<PERCENTAGE_400, percentage_400>,
		EnumValuePair<PERCENTAGE_800, percentage_800>
	>;
};

template <>
struct OvTools::Utils::MappingFor<OvEditor::Panels::EScaleMode, float>
{
	using enum OvEditor::Panels::EScaleMode;
	using type = std::tuple<
		EnumValuePair<PERCENTAGE_5, 0.05f>,
		EnumValuePair<PERCENTAGE_10, 0.1f>,
		EnumValuePair<PERCENTAGE_25, 0.25f>,
		EnumValuePair<PERCENTAGE_50, 0.5f>,
		EnumValuePair<PERCENTAGE_100, 1.0f>,
		EnumValuePair<PERCENTAGE_200, 2.0f>,
		EnumValuePair<PERCENTAGE_400, 4.0f>,
		EnumValuePair<PERCENTAGE_800, 8.0f>
	>;
};

namespace OvEditor::Panels 
{
	void AddOption(OvUI::Widgets::Selection::ComboBox& p_selector, const OvRendering::HAL::Texture& p_texture)
	{
		const auto id = p_texture.GetID();
		p_selector.choices[id] = "Texture " + std::to_string(id) + ": " + p_texture.GetDebugName();
	}

	float CalculateOneToOneScale(const OvMaths::FVector2& p_windowSize, OvRendering::HAL::Texture& p_texture)
	{
		constexpr float kPanelImageMarginX = 45.0f;
		constexpr float kPanelImageMarginY = 120.0f; // Based on the size of the settings above the image

		const auto& texDesc = p_texture.GetDesc();
		float safeSizeX = p_windowSize.x - kPanelImageMarginX;
		float safeSizeY = p_windowSize.y - kPanelImageMarginY;

		return std::min(safeSizeX / texDesc.width, safeSizeY / texDesc.height);
	}

	OvMaths::FVector2 CalculateImageSize(EScaleMode p_mode, const OvMaths::FVector2& p_windowSize, OvRendering::HAL::Texture& p_texture)
	{
		const float scale =
			p_mode == EScaleMode::ONE_TO_ONE ?
			CalculateOneToOneScale(p_windowSize, p_texture) :
			OvTools::Utils::ToValueImpl<EScaleMode, float>(p_mode);

		return {
			p_texture.GetDesc().width * scale,
			p_texture.GetDesc().height * scale
		};
	}

	TextureDebugger::TextureDebugger(
		const std::string& p_title,
		bool p_opened,
		const OvUI::Settings::PanelWindowSettings& p_windowSettings
	) :
		PanelWindow(p_title, p_opened, p_windowSettings),
		m_image(CreateWidget<OvUI::Widgets::Visual::Image>(0, OvMaths::FVector2::Zero)),
		m_textureSelector(CreateWidget<OvUI::Widgets::Selection::ComboBox>()),
		m_scaleSelector(CreateWidget<OvUI::Widgets::Selection::ComboBox>())
	{
		auto& textureRegistry = OVSERVICE(OvEditor::Utils::TextureRegistry);

		allowHorizontalScrollbar = true;

		constexpr int kNoneTextureID = 0;

		m_textureSelector.choices = { {kNoneTextureID, "None"} };

		for (auto& textureID : textureRegistry.GetTextureIDs())
		{
			if (auto texture = textureRegistry.GetTexture(textureID); texture.has_value())
			{
				AddOption(m_textureSelector, texture.value());
			}
		}

		m_textureSelector.ValueChangedEvent += [this, &textureRegistry](int p_selectedTextureID)
		{
			if (p_selectedTextureID == kNoneTextureID)
			{
				m_image.textureID = { 0U };
				m_image.size = OvMaths::FVector2::Zero;
			}
			else
			{
				if (auto texture = textureRegistry.GetTexture(p_selectedTextureID))
				{
					m_selectedTexture = texture;
					m_image.textureID = { texture->GetID() };
					m_image.size = CalculateImageSize(m_scaleMode, GetSize(), texture.value());
				}
			}
		};

		// Selecting the last texture, by default
		if (m_textureSelector.choices.size() > 1)
		{
			m_textureSelector.currentChoice = static_cast<int>(m_textureSelector.choices.size() - 1ULL);
			m_textureSelector.ValueChangedEvent.Invoke(m_textureSelector.currentChoice);
		}

		m_creationListenerID = textureRegistry.textureAddedEvent += [this](const auto& p_desc)
		{
			if (p_desc.id != 0)
			{
				m_textureSelector.choices[p_desc.id] = "Texture " + std::to_string(p_desc.id) + ": " + p_desc.texture->GetDebugName();
				AddOption(m_textureSelector, *p_desc.texture);
			}
		};

		m_destructionListenerID = textureRegistry.textureRemovedEvent += [this](const auto& p_desc)
		{
			if (p_desc.id != 0)
			{
				m_textureSelector.choices.erase(p_desc.id);
				if (m_textureSelector.currentChoice == p_desc.id)
				{
					m_textureSelector.currentChoice = 0;
					m_textureSelector.ValueChangedEvent.Invoke(0);
					m_selectedTexture = std::nullopt;
				}
			}
		};

		auto scaleEntry = [](auto p_mode) {
			return std::pair{
				static_cast<int>(p_mode),
				OvTools::Utils::ToValueImpl<EScaleMode, std::string>(p_mode)
			};
		};

		using enum EScaleMode;
		m_scaleSelector.choices = {
			scaleEntry(ONE_TO_ONE),
			scaleEntry(PERCENTAGE_5),
			scaleEntry(PERCENTAGE_10),
			scaleEntry(PERCENTAGE_25),
			scaleEntry(PERCENTAGE_50),
			scaleEntry(PERCENTAGE_100),
			scaleEntry(PERCENTAGE_200),
			scaleEntry(PERCENTAGE_400),
			scaleEntry(PERCENTAGE_800)
		};
		m_scaleSelector.currentChoice = 0;
		m_scaleSelector.ValueChangedEvent += [this](int p_selectedScale) {
			m_scaleMode = static_cast<EScaleMode>(p_selectedScale);
			// Reset the image size if no texture is selected
			if (m_selectedTexture)
			{
				m_image.size = CalculateImageSize(m_scaleMode, GetSize(), m_selectedTexture.value());
			}
			else
			{
				m_image.size = OvMaths::FVector2::Zero;
			}
		};
	}
	TextureDebugger::~TextureDebugger()
	{
		OvRendering::HAL::Texture::CreationEvent -= m_creationListenerID;
		OvRendering::HAL::Texture::DestructionEvent -= m_destructionListenerID;
	}

	void TextureDebugger::Update(float p_deltaTime)
	{
		if (m_selectedTexture)
		{
			m_image.size = CalculateImageSize(m_scaleMode, GetSize(), m_selectedTexture.value());
		}
	}
}
