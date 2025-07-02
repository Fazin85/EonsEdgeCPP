#pragma once

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "texture.h"

namespace Eon
{
	struct Slot
	{
		explicit Slot(int x, int y, int width, int height);

		bool CanFit(int w, int h) const;
		bool Overlaps(const Slot& other) const;

		bool operator==(const Slot& other) const = default;

		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	};
};

namespace std
{
	template <> struct hash<Eon::Slot>
	{
		size_t operator()(const Eon::Slot& s) const noexcept
		{
			std::size_t hash = 0;
			hash ^= std::hash<int>{}(s.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= std::hash<int>{}(s.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= std::hash<int>{}(s.width) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= std::hash<int>{}(s.height) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			return hash;
		}
	};
};

namespace Eon
{
	class TextureAtlasStitcher
	{
	public:

		class TextureHolder
		{
		public:
			TextureHolder(const std::string& name, sf::Image& image);

			sf::Image& GetImage() const;
			int GetWidth() const;
			int GetHeight() const;
			int GetAtlasX() const;
			void SetAtlasX(int x);
			int GetAtlasY() const;
			void SetAtlasY(int y);

			float GetU0(float atlasWidth) const;
			float GetV0(float atlasHeight) const;
			float GetU1(float atlasWidth) const;
			float GetV1(float atlasHeight) const;

		private:
			std::string name;
			sf::Image& image;
			const int width;
			const int height;
			int atlas_x = 0;
			int atlas_y = 0;
		};

		void AddSprite(const std::string& name, sf::Image& image);
		void DoStitch();
		std::unique_ptr<sf::Image> StitchImages();
		std::unique_ptr<Texture> CreateAtlasTexture();
		std::shared_ptr<TextureHolder> GetHolder(const std::string& name);
		glm::ivec2 GetCurrentSize() const;
			
	private:
		bool TryStitch(const std::vector<std::shared_ptr<TextureHolder>>& holderList);
		bool AllocateSlot(TextureHolder& holder);
		void SplitSlot(const Slot& slot, int usedWidth, int usedHeight);
		bool OverlapsExisting(const Slot& newSlot) const;
		void InsertSorted(const Slot& slot);
		int NextPowerOfTwo(int value) const;

		std::unordered_set<Slot> slots;
		std::vector<Slot> slot_list;
		std::unordered_map<std::string, std::shared_ptr<TextureHolder>> holders;
		int current_width = 0;
		int current_height = 0;
		int max_width = 0;
		int max_height = 0;
		bool force_power_of_two = true;
	};
}