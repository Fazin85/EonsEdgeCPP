#include "texture_atlas_stitcher.h"
#include "log.h"

namespace Eon
{
	TextureAtlasStitcher::TextureHolder::TextureHolder(const std::string& name, sf::Image* image) : name(name), image(image)
	{
	}

	sf::Image* TextureAtlasStitcher::TextureHolder::GetImage() const
	{
		return image;
	}

	int TextureAtlasStitcher::TextureHolder::GetWidth() const
	{
		return width;
	}

	int TextureAtlasStitcher::TextureHolder::GetHeight() const
	{
		return height;
	}

	int TextureAtlasStitcher::TextureHolder::GetAtlasX() const
	{
		return atlas_x;
	}

	void TextureAtlasStitcher::TextureHolder::SetAtlasX(int x)
	{
		atlas_x = x;
	}

	int TextureAtlasStitcher::TextureHolder::GetAtlasY() const
	{
		return atlas_y;
	}

	void TextureAtlasStitcher::TextureHolder::SetAtlasY(int y)
	{
		atlas_y = y;
	}

	float TextureAtlasStitcher::TextureHolder::GetU0(float atlasWidth) const
	{
		return static_cast<float>(atlas_x) / atlasWidth;
	}

	float TextureAtlasStitcher::TextureHolder::GetV0(float atlasHeight) const
	{
		return static_cast<float>(atlas_y) / atlasHeight;
	}

	float TextureAtlasStitcher::TextureHolder::GetU1(float atlasWidth) const
	{
		return static_cast<float>(atlas_x + width) / atlasWidth;
	}

	float TextureAtlasStitcher::TextureHolder::GetV1(float atlasHeight) const
	{
		return static_cast<float>(atlas_y + height) / atlasHeight;
	}

	Slot::Slot(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
	{
	}

	bool Slot::CanFit(int w, int h) const
	{
		return width >= w && height >= h;
	}

	bool Slot::Overlaps(const Slot& other) const
	{
		return x < other.x + other.width &&
			x + width > other.x &&
			y < other.y + other.height &&
			y + height > other.y;
	}

	void TextureAtlasStitcher::AddSprite(const std::string& name, sf::Image& image)
	{
		if (holders.contains(name))
		{
			EON_ERROR("Texture with name " + name + " already exists.");
			return;
		}

		holders[name] = std::make_shared<TextureHolder>(name, &image);

		int w = image.getSize().x;
		int h = image.getSize().y;

		max_width = std::max(max_width, w);
		max_height = std::max(max_height, h);
	}

	void TextureAtlasStitcher::DoStitch()
	{
		std::vector<std::shared_ptr<TextureHolder>> holderList;

		for (const auto& [name, holder] : holders)
		{
			holderList.push_back(holder);
		}

		std::sort(holderList.begin(), holderList.end(), [](const std::shared_ptr<TextureHolder>& a, const std::shared_ptr<TextureHolder>& b)
			{
				int areaA = a->GetWidth() * a->GetHeight();
				int areaB = b->GetWidth() * b->GetHeight();
				if (areaA != areaB)
				{
					return areaB < areaA;
				}
				if (a->GetHeight() != b->GetHeight())
				{
					return b->GetHeight() < a->GetHeight();
				}
				return b->GetWidth() < a->GetWidth();
			});

		int totalArea = 0;
		for (const std::shared_ptr<TextureHolder>& holder : holderList)
		{
			totalArea += holder->GetWidth() * holder->GetHeight();
		}

		current_width = std::max(max_width, NextPowerOfTwo(static_cast<int>(std::ceil(std::sqrt(totalArea)))));
		current_height = std::max(max_height, NextPowerOfTwo(static_cast<int>(std::ceil(std::sqrt(totalArea)))));

		bool allFit = false;
		int attempts = 0;

		while (!allFit && attempts < 10)
		{
			allFit = TryStitch(holderList);

			if (!allFit)
			{
				if (current_width <= current_height)
				{
					current_width *= 2;
				}
				else
				{
					current_width *= 2;
				}
				attempts++;

				slots.clear();
				slot_list.clear();
				for (std::shared_ptr<TextureHolder>& holder : holderList)
				{
					holder->SetAtlasX(0);
					holder->SetAtlasY(0);
				}
			}
		}

		if (!allFit)
		{
			EON_ERROR("Could not fit all textures in atlas.");
		}
	}

	bool TextureAtlasStitcher::TryStitch(std::vector<std::shared_ptr<TextureHolder>>& holderList)
	{
		Slot rootSlot(0, 0, current_width, current_height);
		slots.insert(rootSlot);
		slot_list.push_back(rootSlot);

		for (std::shared_ptr<TextureHolder>& holder : holderList)
		{
			if (!AllocateSlot(*holder))
			{
				return false;
			}
		}
		return true;
	}

	bool TextureAtlasStitcher::AllocateSlot(TextureHolder& holder)
	{
		int width = holder.GetImage()->getSize().x;
		int height = holder.GetImage()->getSize().y;

		for (int i = 0; i < slot_list.size(); i++)
		{
			const Slot& slot = slot_list[i];
			if (slot.CanFit(width, height))
			{
				holder.SetAtlasX(slot.x);
				holder.SetAtlasY(slot.y);

				slot_list.erase(slot_list.begin() + i);
				slots.erase(slot);

				SplitSlot(slot, width, height);
				return true;
			}
		}
		return false;
	}

	void TextureAtlasStitcher::SplitSlot(const Slot& slot, int usedWidth, int usedHeight)
	{
		int remainingWidth = slot.width - usedWidth;
		int remainingHeight = slot.height - usedHeight;

		if (remainingWidth > 0)
		{
			const Slot& rightSlot = Slot(slot.x + usedWidth, slot.y, remainingWidth, slot.height);
			if (!OverlapsExisting(rightSlot))
			{
				slots.insert(rightSlot);
				InsertSorted(rightSlot);
			}
		}

		if (remainingHeight > 0)
		{
			const Slot& bottomSlot = Slot(slot.x, slot.y + usedHeight, usedWidth, remainingHeight);
			if (!OverlapsExisting(bottomSlot))
			{
				slots.insert(bottomSlot);
				InsertSorted(bottomSlot);
			}
		}
	}

	bool TextureAtlasStitcher::OverlapsExisting(const Slot& newSlot) const
	{
		for (const Slot& existing : slot_list)
		{
			if (newSlot.Overlaps(existing))
			{
				return true;
			}
		}
		return false;
	}

	void TextureAtlasStitcher::InsertSorted(const Slot& slot)
	{
		const int area = slot.width * slot.height;
		int insertIndex = 0;

		for (int i = 0; i < slot_list.size(); i++)
		{
			const Slot& existing = slot_list[i];
			const int existingArea = existing.width * existing.height;

			if (area <= existingArea)
			{
				insertIndex = i + 1;
			}
			else
			{
				break;
			}
		}

		slot_list.insert(slot_list.begin() + insertIndex, slot);
	}

	int TextureAtlasStitcher::NextPowerOfTwo(int value) const
	{
		if (!force_power_of_two)
		{
			return value;
		}

		int power = 1;
		while (power < value)
		{
			power *= 2;
		}
		return power;
	}

	sf::Image* TextureAtlasStitcher::StitchImages()
	{
		return nullptr;
	}
}