#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <any>
#include "shader.h"
#include "texture.h"
#include "st_shared_ptr.h"

namespace Eon
{
	template <typename T> class Asset;
	class AssetManager;

	class AssetID
	{
	public:
		static const AssetID INVALID_ASSET_ID;

		AssetID(int id) : id(id) {}

		AssetID() {}

		bool operator==(const AssetID& other) const = default;

		int operator*() { return id; }

		int operator*() const { return id; }

		template<typename T>
		Asset<T> Get() const
		{
			return AssetManager::GetAssetByID<T>(*this);
		}

		bool IsValid() const { return id != INVALID_ID; }

	private:
		constexpr static int INVALID_ID = -1;

		int id = INVALID_ID;
	};

	using ShaderID = AssetID;
	using TextureID = AssetID;

	template <typename T>
	class Asset
	{
	public:
		Asset() = default;

		Asset(Asset& other) : asset(other.asset), id(other.id) {}

		Asset(T&& asset, AssetID id) : asset(make_shared_st<T>(std::move(asset))), id(id) {}

		Asset(Asset&& other) noexcept : asset(other.asset), id(other.id)
		{
			other.asset = nullptr;
			other.id = AssetID::INVALID_ASSET_ID;
		}

		bool operator==(const Asset& other) const
		{
			return id == other.id;
		}

		T& operator*() { return *asset; }

		const T& operator*() const { return *asset; }

		T* operator->() { return asset.get(); }

		const T* operator->() const { return asset.get(); }

		T* Get() { return asset.get(); }

		AssetID GetID() const { return id; }

		bool IsValid() const { return asset != nullptr && id != AssetID::INVALID_ASSET_ID; }
	private:
		st_shared_ptr<T> asset = nullptr;
		AssetID id = AssetID::INVALID_ASSET_ID;
	};

	class AssetManager
	{
	public:
		struct AssetEntry
		{
			std::string name;           // Namespaced name like "characters.player.idle"
			std::string type;           // Asset type: "texture", "sound", "model", etc.
			std::string filepath;       // Path to the asset file
		};

		template <typename T>
		static Asset<T> GetAsset(const std::string& name)
		{
			static constexpr bool isAllowedType =
				std::is_same_v<T, Texture> ||
				std::is_same_v<T, Shader>;

			static_assert(isAllowedType, "Asset type must be Texture or Shader");

			if constexpr (std::is_same_v<T, Texture>)
			{
				if (auto it = GetInstance().textures_by_namespace.find(name); it != GetInstance().textures_by_namespace.end())
				{
					return it->second;
				}
				throw std::runtime_error("Texture not found: " + name);
			}
			else if constexpr (std::is_same_v<T, Shader>)
			{
				if (auto it = GetInstance().shaders_by_namespace.find(name); it != GetInstance().shaders_by_namespace.end())
				{
					return it->second;
				}
				throw std::runtime_error("Shader not found: " + name);
			}

			return Asset<T>();
		}

		template <typename T>
		static Asset<T> GetAssetByID(AssetID id)
		{
			static constexpr bool isAllowedType =
				std::is_same_v<T, Texture> ||
				std::is_same_v<T, Shader>;

			static_assert(isAllowedType, "Asset type must be Texture or Shader");

			if constexpr (std::is_same_v<T, Texture>)
			{
				if (*id < 0 || *id > GetInstance().textures_by_id.size())
				{
					throw std::runtime_error("Texture not found: " + std::to_string(*id));
				}

				return GetInstance().textures_by_id[*id];
			}
			else if constexpr (std::is_same_v<T, Shader>)
			{
				if (*id < 0 || *id > GetInstance().shaders_by_id.size())
				{
					throw std::runtime_error("Shader not found: " + std::to_string(*id));
				}

				return GetInstance().shaders_by_id[*id];
			}

			return Asset<T>();
		}

		template <typename T>
		static AssetID GetAssetID(const std::string& name)
		{
			if (auto asset = GetAsset<T>(name); asset.IsValid())
			{
				return asset.GetID();
			}

			return AssetID::INVALID_ASSET_ID;
		}

	private:
		static AssetManager& GetInstance();
		void LoadAssets(const std::string& manifestPath);

		std::unordered_map<std::string, Asset<Shader>> shaders_by_namespace;
		std::unordered_map<std::string, Asset<Texture>> textures_by_namespace;
		std::vector<Asset<Shader>> shaders_by_id;
		std::vector<Asset<Texture>> textures_by_id;
		int next_shader_id = 0;
		int next_texture_id = 0;
	};
}

namespace std
{
	template<>
	struct hash<Eon::AssetID>
	{
		size_t operator()(const Eon::AssetID& asset_id) const noexcept
		{
			return std::hash<int>{}(*asset_id);
		}
	};
}