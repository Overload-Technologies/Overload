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

#include <OvMaths/FMatrix3.h>

#include <OvRendering/Data/StateMask.h>
#include <OvRendering/HAL/TextureHandle.h>
#include <OvRendering/Resources/Shader.h>
#include <OvRendering/Resources/Texture.h>
#include <OvRendering/Settings/EProjectionMode.h>

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
		OvMaths::FMatrix3,
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
	* Context required by materials when bound.
	*/
	struct MaterialBindContext
	{
		HAL::Texture* emptyTexture2D = nullptr; // p_emptyTexture2D (The texture to use if a texture uniform is null)
		HAL::Texture* emptyTextureCube = nullptr; // p_emptyTextureCube (The texture to use if a texture uniform is null)
		std::optional<const std::string_view> pass = std::nullopt;
		OvTools::Utils::OptRef<const Data::FeatureSet> featureSetOverride = std::nullopt;
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
		* Returns the variant (shader program) given a pass and the current feature set (or an override)
		* @param p_pass
		* @param p_override
		*/
		OvTools::Utils::OptRef<OvRendering::HAL::ShaderProgram> GetVariant(
			std::optional<const std::string_view> p_pass = std::nullopt,
			OvTools::Utils::OptRef<const Data::FeatureSet> p_override = std::nullopt
		) const;

		/**
		* Add missing properties to the material based on the shader, and remove properties that are not used in the shader.
		*/
		void UpdateProperties();

		/**
		* Calculate a hash for the material's stable (non-mutable) bind state, used for caching purposes.
		* This hash is always computed regardless of whether mutable properties are present.
		* @param p_bindContext
		*/
		[[nodiscard]] std::size_t CalculateBindContextHash(
			const MaterialBindContext& p_bindContext
		) const;

		/**
		* Returns true if the material has mutable (singleUse) properties pending upload.
		*/
		[[nodiscard]] bool HasMutableProperties() const;

		/**
		* Bind the material and send its uniform data to the GPU
		* @param p_bindContext
		*/
		void Bind(
			const MaterialBindContext& p_bindContext
		);

		/**
		* Re-upload only the mutable (singleUse) properties to an already-bound program.
		* Call this instead of Bind() when the stable bind context hash hasn't changed
		* but HasMutableProperties() is true.
		* @param p_bindContext
		*/
		void BindMutableProperties(
			const MaterialBindContext& p_bindContext
		);

		/**
		* Unbind the material
		*/
		void Unbind() const;

		/**
		* Returns true if the material has a given property
		* @param p_name
		*/
		bool HasProperty(const std::string& p_name) const;

		/**
		* Sets a material property value
		* @note This method expects that the property already exists. If unsure, use TrySetProperty instead.
		* @param p_name
		* @param p_value
		* @param p_singleUse (automatically consume the value after the first use)
		*/
		void SetProperty(const std::string p_name, const MaterialPropertyType& p_value, bool p_singleUse = false);

		/**
		* Sets a material property value if the property exists
		* @param p_name
		* @param p_value
		* @param p_singleUse (automatically consume the value after the first use)
		*/
		bool TrySetProperty(const std::string& p_name, const MaterialPropertyType& p_value, bool p_singleUse = false);

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
		* Sets the shader support for orthographic projection
		* @param p_supportOrthographic
		*/
		void SetOrthographicSupport(bool p_supportOrthographic);

		/**
		* Sets the shader support for perspective projection
		* @param p_supportPerspective
		*/
		void SetPerspectiveSupport(bool p_supportPerspective);

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
		* Sets whether this material should be captured by reflection probes
		* @param p_capturedByReflectionProbes
		*/
		void SetCapturedByReflectionProbes(bool p_capturedByReflectionProbes);

		/**
		* Sets whether this material should receive reflections from reflection probes
		* @param p_receiveReflections
		*/
		void SetReceiveReflections(bool p_receiveReflections);

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
		* Returns true if the material is set to be captured by reflection probes
		*/
		bool IsCapturedByReflectionProbes() const;

		/**
		* Returns true if the material is set to receive reflections
		*/
		bool IsReflectionReceiver() const;

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
		Data::FeatureSet& GetFeatures();

		/**
		* Defines the feature set this material should use
		* @param p_features
		*/
		void SetFeatures(const Data::FeatureSet& p_features);

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

		/**
		* Returns true if the material has a pass
		* @param p_pass
		*/
		bool HasPass(const std::string& p_pass) const;

		/**
		* Returns true if the material supports orthopgraphic projection
		*/
		bool SupportsOrthographic() const;

		/**
		* Returns true if the material supports perspective projection
		*/
		bool SupportsPerspective() const;

		/**
		* Returns true if the material supports the given projection mode
		* @param p_projectionMode
		*/
		bool SupportsProjectionMode(OvRendering::Settings::EProjectionMode p_projectionMode) const;

	protected:
		void UpdateBindCacheVersion();

	protected:
		OvRendering::Resources::Shader* m_shader = nullptr;
		PropertyMap m_properties;
		Data::FeatureSet m_features;
		bool m_hasMutableProperties = false;

		bool m_supportOrthographic = true;
		bool m_supportPerspective = true;
		bool m_userInterface = false;

		bool m_blendable = false;
		bool m_backfaceCulling = true;
		bool m_frontfaceCulling = false;
		bool m_depthTest = true;
		bool m_depthWriting = true;
		bool m_colorWriting = true;

		bool m_castShadows = true;
		bool m_receiveShadows = true;
		bool m_capturedByReflectionProbes = true;
		bool m_receiveReflections = true;

		int m_gpuInstances = 1;
		int m_drawOrder = 1000;

		// For caching purposes. If the material properties changed between 2 binds,
		// this will ensure the material hash also changes.
		std::size_t m_bindCacheVersion = 0;
	};
}
