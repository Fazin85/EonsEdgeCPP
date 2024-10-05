#pragma once

#include <array>

#include "block.h"
#include "chunk.h"
#include "chunk_position.h"
#include "num.h"

class Level {
  Chunk* GetChunk(ChunkPosition position);
  Block* GetBlock(u16 x, u16 y, u16 z);

 private:
  std::array<Chunk*, 1024> chunks;
  u32 IndexFromPosition(u16 x, u16 z);
};