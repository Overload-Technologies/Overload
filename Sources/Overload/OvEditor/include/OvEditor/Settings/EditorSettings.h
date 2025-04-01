/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Eventing/Event.h>
#include <OvUI/Styling/EStyle.h>

namespace OvEditor::Settings
{
	/**
	* Accessible from anywhere editor settings
	*/
	class EditorSettings
	{
	public:
		template<typename T>
		class Property
		{
		public:
			/**
			* Creates the property with a default value
			* @param p_value
			*/
			Property(T p_value) : m_value(p_value) {}

			/**
			* Event called when the property value changes
			*/
			OvTools::Eventing::Event<T> OnValueChanged;

			/**
			* Assign a new value to the property
			* @param p_value
			*/
			inline T& operator=(T p_value)
			{
				Set(p_value);
				return m_value;
			}

			/**
			* Assign a new valeu to the property
			* @param p_value
			*/
			inline void Set(T p_value)
			{
				m_value = p_value;
				OnValueChanged.Invoke(m_value);
			}

			inline operator T()
			{
				return m_value;
			}

			/**
			* Returns the value of the property
			*/
			inline T Get() const
			{
				return m_value;
			}

		private:
			T m_value;
		};

		/**
		* No construction possible
		*/
		EditorSettings() = delete;

		/**
		* Save the settings
		*/
		static void Save();

		/**
		* Load the settings
		*/
		static void Load();

		inline static Property<bool> ShowGeometryBounds = { false };
		inline static Property<bool> ShowLightBounds = { false };
		inline static Property<bool> EditorFrustumGeometryCulling = { true };
		inline static Property<bool> EditorFrustumLightCulling = { true };
		inline static Property<bool> DebugFrustumCulling = { false };
		inline static Property<float> LightBillboardScale = { 0.5f };
		inline static Property<float> TranslationSnapUnit = { 1.0f };
		inline static Property<float> RotationSnapUnit = { 15.0f };
		inline static Property<float> ScalingSnapUnit = { 1.0f };
		inline static Property<int> ColorTheme = { static_cast<int>(OvUI::Styling::EStyle::DEFAULT_DARK) };
	};
}
