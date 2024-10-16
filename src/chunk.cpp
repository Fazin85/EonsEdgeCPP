#include "chunk.h"
#include <lz4.h>
#include "log.h"

namespace Eon
{
	Chunk::Chunk(ChunkPosition chunkPosition) : position(chunkPosition)
	{
		blocks = new Block[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH];
		compressed = false;
		compressed_blocks = nullptr;
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

	Block* Chunk::GetBlock(u8 x, u16 y, u8 z)
	{
		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 || z < 0 || y < 0)
		{
			return nullptr;
		};

		return &blocks[IndexFromPosition(x, y, z)];
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

		if (decompressedSize != SrcSize || decompressedSize < 0)
		{
			EON_ERROR("Failed to decompress chunk");
		}

		compressed_blocks_size = 0;
		compressed = false;
	}

	u32 Chunk::IndexFromPosition(u8 x, u16 y, u8 z)
	{
		return x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
	}
} // namespace Eon
