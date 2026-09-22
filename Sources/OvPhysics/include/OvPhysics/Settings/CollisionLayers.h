/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <OvTools/Filesystem/IniFile.h>

namespace OvPhysics::Settings
{
	/**
	* Holds the collision layers of a project, and the matrix defining which layers collide together.
	* Layers are fixed slots: naming a free slot creates a layer, clearing its name frees it back, and
	* indices never move, so objects referencing a layer stay on it whatever happens to the others
	*/
	class CollisionLayers
	{
	public:
		/* Bullet filters collisions with 32 bit masks, which caps the number of slots */
		static constexpr uint32_t kMaxLayerCount = 32;
		static constexpr uint32_t kDefaultLayer = 0;
		static constexpr std::string_view kDefaultLayerName = "Default";

		/**
		* Creates the default layers, holding the default layer alone
		*/
		CollisionLayers();

		/**
		* Names the first free slot and returns its index.
		* Returns nothing if the name is already taken, invalid, or if every slot is used
		* @param p_name
		*/
		std::optional<uint32_t> AddLayer(const std::string& p_name);

		/**
		* Frees the slot of the given layer, which collides with every layer again.
		* The default layer cannot be removed
		* @param p_layer
		*/
		void RemoveLayer(uint32_t p_layer);

		/**
		* Renames the given layer, naming its slot when it was free.
		* The default layer cannot be renamed
		* @param p_layer
		* @param p_name
		*/
		void RenameLayer(uint32_t p_layer, const std::string& p_name);

		/**
		* Returns true if the given slot holds a layer
		* @param p_layer
		*/
		bool IsLayerUsed(uint32_t p_layer) const;

		/**
		* Returns the name of the given layer, empty when its slot is free
		* @param p_layer
		*/
		const std::string& GetLayerName(uint32_t p_layer) const;

		/**
		* Returns the index of the layer having the given name, or nothing if no layer matches
		* @param p_name
		*/
		std::optional<uint32_t> FindLayer(std::string_view p_name) const;

		/**
		* Defines if the two given layers should collide together
		* @param p_first
		* @param p_second
		* @param p_collide
		*/
		void SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide);

		/**
		* Returns true if the two given layers collide together
		* @param p_first
		* @param p_second
		*/
		bool GetLayerCollision(uint32_t p_first, uint32_t p_second) const;

		/**
		* Returns the collision mask of the given layer
		* @param p_layer
		*/
		uint32_t GetLayerMask(uint32_t p_layer) const;

		/**
		* Restores the layers from the given settings, free slots falling back to their defaults
		* @param p_settings
		*/
		void Deserialize(const OvTools::Filesystem::IniFile& p_settings);

		/**
		* Stores the layers into the given settings, dropping the keys of the free slots
		* @param p_settings
		*/
		void Serialize(OvTools::Filesystem::IniFile& p_settings) const;

	private:
		bool IsNameValid(std::string_view p_name) const;

		std::array<std::string, kMaxLayerCount> m_names;
		std::array<uint32_t, kMaxLayerCount> m_masks;
	};
}
