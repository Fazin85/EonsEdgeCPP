#include "chunk.h"

Chunk::Chunk(ChunkPosition chunkPosition) : blocks{} {
  position = chunkPosition;
}

Block* Chunk::GetBlock(u8 x, u16 y, u8 z) {
  if (x >= 16 || z >= 16 || y >= 512 || x < 0 || z < 0 || y < 0) {
    return nullptr;
  };

  return &blocks[IndexFromPosition(x, y, z)];
}

ChunkPosition Chunk::Position() const { return position; }

u32 Chunk::IndexFromPosition(u8 x, u16 y, u8 z) {
  return x + 16 * (y + 16 * z);
}
