#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "asset_manager.h"
#include "shader.h"
#include "log.h"

namespace Eon
{
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
			size_t firstColon = line.find(':');
			size_t secondColon = line.find(':', firstColon + 1);

			if (firstColon != std::string::npos && secondColon != std::string::npos)
			{
				AssetEntry entry;
				entry.type = line.substr(0, firstColon);
				entry.name = line.substr(firstColon + 1, secondColon - firstColon - 1);
				entry.filepath = line.substr(secondColon + 1);
				entries.emplace_back(entry);
			}
		}

		for (const auto& entry : entries)
		{
			if (entry.type == "texture")
			{
				sf::Image image;
				image.loadFromFile(entry.filepath);
				if (!image.getPixelsPtr())
				{
					throw std::runtime_error("Failed to load texture: " + entry.filepath);
				}

				Texture texture(image, false);
				Asset asset(std::move(texture), next_texture_id++);

				textures_by_namespace.try_emplace(entry.name, asset);
				textures_by_id.emplace_back(asset);
			}
			else if (entry.type == "shader")
			{
				std::string vertexShader = entry.filepath + ".vert";
				std::string fragmentShader = entry.filepath + ".frag";

				Shader shader(vertexShader, fragmentShader, true);

				shaders_by_namespace.try_emplace(entry.name, std::move(shader), next_shader_id++);
			}
		}
	}
}