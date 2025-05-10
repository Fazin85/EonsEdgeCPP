#pragma once

#include "byte_buffer.h"

#include <memory>
#include <glm/glm.hpp>
#include <SFML/Window.hpp>

namespace Eon
{

	class BlockEntity
	{
	public:
		explicit BlockEntity(const glm::ivec3& worldPosition);
		virtual ~BlockEntity() = default;

		virtual void Update(float dt) {}
		virtual void Render() {}
		virtual void OnClicked(sf::Mouse::Button button) {}
		virtual void Save(ByteBuffer& byteBuf) {}
		virtual void Load(ByteBuffer& byteBuf) {}

		glm::ivec3 GetWorldPosition() const;

	protected:
		glm::ivec3 world_position;
	};
}