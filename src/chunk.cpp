#include "chunk.h"
#include <lz4.h>
#include "log.h"

#define INDEX_FROM_POSITION(x, y, z) x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z)

namespace Eon
{
	Chunk::Chunk(ChunkPosition chunkPosition) : position(chunkPosition)
	{
		constexpr size_t blockCount = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH;
		blocks = new Block[blockCount];
		compressed = false;
		compressed_blocks = nullptr;
		compressed_blocks_size = 0;
	}

	Chunk::~Chunk()
	{
		if (compressed && compressed_blocks != nullptr)
		{
			delete[] compressed_blocks;
		}
		else if (blocks != nullptr)
		{
			delete[] blocks;
		}
	}

	Block* Chunk::GetBlock(unsigned char x, short y, unsigned char z)
	{
		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 || z < 0 || y < 0 || compressed || blocks == nullptr)
		{
			return nullptr;
		}

		return &blocks[INDEX_FROM_POSITION(x, y, z)];
	}

	ChunkPosition Chunk::Position() const
	{
		return position;
	}

	constexpr int SrcSize = sizeof(Block) * (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH);

	void Chunk::Compress()
	{
		if (compressed)
		{
			return;
		}

		constexpr int DstSize = LZ4_COMPRESSBOUND(SrcSize);

		compressed_blocks = new char[DstSize];

		compressed_blocks_size = LZ4_compress_default(reinterpret_cast<char*>(blocks), compressed_blocks, SrcSize, DstSize);

		if (compressed_blocks_size <= 0)
		{
			EON_ERROR("Failed to compress chunk");
			return;
		}

		compressed_blocks = reinterpret_cast<char*>(std::realloc(compressed_blocks, compressed_blocks_size));

		delete[] blocks;
		blocks = nullptr;

		compressed = true;
	}

	void Chunk::Decompress()
	{
		if (!compressed)
		{
			return;
		}

		blocks = new Block[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH];

		int decompressedSize = LZ4_decompress_safe(compressed_blocks, reinterpret_cast<char*>(blocks), compressed_blocks_size, SrcSize);

		delete[] compressed_blocks;
		compressed_blocks = nullptr;

		if (decompressedSize != SrcSize || decompressedSize < 0)
		{
			EON_ERROR("Failed to decompress chunk");
		}

		compressed_blocks_size = 0;
		compressed = false;
	}

	void Chunk::DeleteBlocks()
	{
		if (blocks != nullptr)
		{
			delete[] blocks;
			blocks = nullptr;
		}
	}

	Block* Chunk::GetBlocks()
	{
		return blocks;
	}

	short* Chunk::GetHeightestBlockY(unsigned char x, unsigned char z)
	{
		return &highest_blocks[(x * CHUNK_WIDTH) + z];
	}

	std::vector<char> Chunk::CompressToBuffer() const
	{
		constexpr int DstSize = LZ4_COMPRESSBOUND(SrcSize);
		std::vector<char> buffer;
		buffer.reserve(DstSize);

		int size = LZ4_compress_default(reinterpret_cast<char*>(blocks), buffer.data(), SrcSize, DstSize);

		if (size <= 0)
		{
			EON_ERROR("Failed to compress chunk");
			return std::vector<char>();
		}

		buffer.resize(size);
		return buffer;
	}
} // namespace Eon
