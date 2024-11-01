#pragma once

#include "block.h"
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace Eon
{
	class VoxelModel
	{
	public:
		VoxelModel(const std::string& fileName);
		~VoxelModel();
		bool Load(const std::string& fileName);
		const BlockType GetBlockType(short x, short y, short z);
		glm::ivec3 Size() const;

	private:
		BlockType* block_data = nullptr;
		glm::ivec3 size;
	};
}