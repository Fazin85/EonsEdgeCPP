#pragma once

constexpr auto CHUNK_WIDTH = 16;
constexpr auto CHUNK_HEIGHT = 128;
#define CHUNK_BLOCK_COUNT CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
constexpr auto CHUNK_BITSHIFT_AMOUNT = 4;
