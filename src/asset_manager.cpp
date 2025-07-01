#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "asset_manager.h"
#include "shader.h"
#include "log.h"

namespace Eon
{
	const AssetID AssetID::INVALID_ASSET_ID = { AssetID::INVALID_ID };

	AssetManager& AssetManager::GetInstance()
	{
		static AssetManager instance;

		if (static bool initialized = false; !initialized)
		{
			instance.LoadAssets("content/manifest.txt");

			initialized = true;
		}
		return instance;
	}

	void AssetManager::LoadAssets(const std::string& manifestPath)
	{
		std::ifstream file(manifestPath);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open manifest file: " + manifestPath);
		}

		std::string content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		std::vector<AssetEntry> entries;
		std::istringstream stream(content);
		std::string line;

		while (std::getline(stream, line))
		{
			if (line.empty() || line[0] == '#') continue;

			// Parse line format: "type:namespaced.name:filepath"
			std::vector<std::string> parts = Split(line, ':');

			if (parts.size() < 3)
			{
				throw std::runtime_error("Invalid manifest entry: " + line);
			}

			AssetEntry entry;
			entry.type = parts[0];
			entry.values = std::vector<std::string>(parts.begin() + 1, parts.end());
			entries.emplace_back(entry);
		}

		for (const auto& entry : entries)
		{
			if (entry.type == "texture")
			{
				sf::Image image;
				image.loadFromFile(entry.FilePath());
				if (!image.getPixelsPtr())
				{
					throw std::runtime_error("Failed to load texture: " + entry.FilePath());
				}

				Texture texture(image, false);
				Asset asset(std::move(texture), { next_texture_id++ });

				textures_by_namespace.try_emplace(entry.Name(), asset);
				textures_by_id.emplace_back(asset);
			}
			else if (entry.type == "shader")
			{
				std::string vertexShader = entry.values[1];
				std::string fragmentShader = entry.values[2];

				Shader shader(vertexShader, fragmentShader, true);

				Asset asset(std::move(shader), { next_shader_id++ });

				shaders_by_namespace.try_emplace(entry.Name(), asset);
				shaders_by_id.emplace_back(asset);
			}
		}
	}

	std::vector<std::string> AssetManager::Split(const std::string& str, char delimiter) const
	{
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string token;

		while (std::getline(ss, token, delimiter))
		{
			tokens.push_back(token);
		}

		return tokens;
	}
}