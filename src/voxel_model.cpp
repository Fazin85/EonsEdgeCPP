#include "voxel_model.h"
#define OGT_VOX_IMPLEMENTATION
#include "ogt_vox.h"

namespace Eon
{
	VoxelModel::VoxelModel(const std::string& fileName)
	{
		Load(fileName);
	}

	VoxelModel::~VoxelModel()
	{
		delete[] block_data;
	}

	bool VoxelModel::Load(const std::string& fileName)
	{
		std::ifstream file("content/models/" + fileName, std::ios::binary);
		std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

		const ogt_vox_scene* scene = ogt_vox_read_scene(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
		const ogt_vox_model* model = scene->models[0];
		size = glm::ivec3(model->size_x, model->size_y, model->size_z);

		block_data = new BlockType[size.x * size.y * size.z];

		for (int i = 0; i < size.x * size.y * size.z; i++)
		{
			block_data[i] = static_cast<BlockType>(model->voxel_data[i]);
		}

		ogt_vox_destroy_scene(scene);

		return true;
	}

	const BlockType VoxelModel::GetBlockType(short x, short y, short z)
	{
		if (block_data == nullptr)
		{
			return BlockType::AIR;
		}

		return block_data[x + z * size.x + y * size.x * size.y];
	}

	glm::ivec3 VoxelModel::Size() const
	{
		return size;
	}
}
