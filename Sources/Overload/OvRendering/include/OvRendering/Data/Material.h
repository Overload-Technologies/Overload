/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <any>
#include <map>
#include <optional>
#include <variant>

#include <OvRendering/Data/StateMask.h>
#include <OvRendering/HAL/TextureHandle.h>
#include <OvRendering/Resources/Shader.h>
#include <OvRendering/Resources/Texture.h>

namespace OvRendering::Data
{
	using MaterialPropertyType = std::variant<
		std::monostate,
		bool,
		int,
		float,
		OvMaths::FVector2,
		OvMaths::FVector3,
		OvMaths::FVector4,
		OvMaths::FMatrix4,
		OvRendering::HAL::TextureHandle*,	// Texture handle
		OvRendering::Resources::Texture*	// Texture asset (serializable)
	>;

	/**
	* Represents a material property to be used as a shader uniform
	*/
	struct MaterialProperty
	{
		MaterialPropertyType value;
		bool singleUse;
	};

	/**
	* A material is a combination of a shader and some settings (Material settings and shader settings)
	*/
	class Material
	{
	public:
		using PropertyMap = std::map<std::string, MaterialProperty>;

		/**
		* Creates a material
		* @param p_shader
		*/
		Material(OvRendering::Resources::Shader* p_shader = nullptr);

		/**
		* Defines the shader to attach to this material instance
		* @param p_shader
		*/
		void SetShader(OvRendering::Resources::Shader* p_shader);

		/**
		* Fill uniform with default uniform values
		*/
		void FillUniform();

		/**
		* Bind the material and send its uniform data to the GPU
		* @param p_emptyTexture (The texture to use if a texture uniform is null)
		* @param p_featureSetOverride
		*/
		void Bind(
			HAL::Texture* p_emptyTexture = nullptr,
			OvTools::Utils::OptRef<const Resources::Shader::FeatureSet> p_featureSetOverride = std::nullopt
		);

		/**
		* Unbind the material
		*/
		void Unbind() const;

		/**
		* Sets a material property value
		* @param p_name
		* @param p_value
		* @param p_singleUse (automatically consume the value after the first use)
		*/
		void SetProperty(const std::string p_name, const MaterialPropertyType& p_value, bool p_singleUse = false);

		/**
		* Gets a material property
		* @param p_name
		*/
		OvTools::Utils::OptRef<const MaterialProperty> GetProperty(const std::string p_name) const;

		/**
		* Returns the attached shader
		*/
		OvRendering::Resources::Shader*& GetShader();

		/**
		* Returns true if the material has a shader attached
		*/
		bool HasShader() const;

		/**
		* Returns true if the material is valid
		*/
		bool IsValid() const;

		/**
		* Sets the draw order of the material
		* @param p_order
		*/
		void SetDrawOrder(int p_order);

		/**
		* Defines if the material is blendable
		* @param p_blendable
		*/
		void SetBlendable(bool p_blendable);

		/**
		* Defines if the material is used on UI
		* @param p_blendable
		*/
		void SetUserInterface(bool p_userInterface);


		/**
		* Defines if the material has backface culling
		* @param p_backfaceCulling
		*/
		void SetBackfaceCulling(bool p_backfaceCulling);

		/**
		* Defines if the material has frontface culling
		* @param p_frontfaceCulling
		*/
		void SetFrontfaceCulling(bool p_frontfaceCulling);

		/**
		* Defines if the material has depth test
		* @param p_depthTest
		*/
		void SetDepthTest(bool p_depthTest);

		/**
		* Defines if the material has depth writting
		* @param p_depthWriting
		*/
		void SetDepthWriting(bool p_depthWriting);

		/**
		* Defines if the material has color writting
		* @param p_colorWriting
		*/
		void SetColorWriting(bool p_colorWriting);

		/**
		* Sets the shadow casting state of the material
		* @param p_castShadows
		*/
		void SetCastShadows(bool p_castShadows);

		/**
		* Sets the shadow receiving state of the material
		* @param p_receiveShadows
		*/
		void SetReceiveShadows(bool p_receiveShadows);

		/**
		* Defines the number of instances
		* @param p_instances
		*/
		void SetGPUInstances(int p_instances);

		/**
		* Returns the draw order of the material
		*/
		int GetDrawOrder() const;

		/**
		* Returns true if the material is blendable
		*/
		bool IsBlendable() const;

		/**
		* Returns true if the material will be used on UI
		*/
		bool IsUserInterface() const;

		/**
		* Returns true if the material has backface culling
		*/
		bool HasBackfaceCulling() const;

		/**
		* Returns true if the material has frontface culling
		*/
		bool HasFrontfaceCulling() const;

		/**
		* Returns true if the material has depth test
		*/
		bool HasDepthTest() const;

		/**
		* Returns true if the material has depth writing
		*/
		bool HasDepthWriting() const;

		/**
		* Returns true if the material has color writing
		*/
		bool HasColorWriting() const;

		/**
		* Returns true if the material is set to cast shadows
		*/
		bool IsShadowCaster() const;

		/**
		* Returns true if the material is set to receive shadows
		*/
		bool IsShadowReceiver() const;

		/**
		* Returns the number of instances
		*/
		int GetGPUInstances() const;

		/**
		* Generate a state mask with the current material settings
		*/
		const StateMask GenerateStateMask() const;

		/**
		* Returns the uniforms data of the material
		*/
		PropertyMap& GetProperties();

		/**
		* Returns the feature set of this material
		*/
		Resources::Shader::FeatureSet& GetFeatures();

		/**
		* Adds a feature to the material
		* @param p_feature
		*/
		void AddFeature(const std::string& p_feature);

		/**
		* Removes a feature from the material
		* @param p_feature
		*/
		void RemoveFeature(const std::string& p_feature);

		/**
		* Returns true if the material has a feature
		* @param p_feature
		*/
		bool HasFeature(const std::string& p_feature) const;

		/**
		* Returns true if the material supports a feature
		* @param p_feature
		*/
		bool SupportsFeature(const std::string& p_feature) const;

	protected:
		OvRendering::Resources::Shader* m_shader = nullptr;
		PropertyMap m_properties;
		Resources::Shader::FeatureSet m_features;

		bool m_userInterface = false;
		bool m_blendable = false;
		bool m_backfaceCulling = true;
		bool m_frontfaceCulling = false;
		bool m_depthTest = true;
		bool m_depthWriting = true;
		bool m_colorWriting = true;
		bool m_castShadows = false;
		bool m_receiveShadows = false;

		int m_gpuInstances = 1;
		int m_drawOrder = 1000;
	};
}
