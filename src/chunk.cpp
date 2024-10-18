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
		if (compressed)
		{
			delete[] compressed_blocks;
		}
		else
		{
			delete[] blocks;
		}
	}

	Block* Chunk::GetBlock(i8 x, i16 y, i8 z)
	{
		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 || z < 0 || y < 0)
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
} // namespace Eon
