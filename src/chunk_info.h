#pragma once

constexpr auto CHUNK_WIDTH = 32;
constexpr auto CHUNK_HEIGHT = 256;
#define CHUNK_BLOCK_COUNT CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
constexpr auto CHUNK_BITSHIFT_AMOUNT = 5;
