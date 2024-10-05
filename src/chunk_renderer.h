#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "num.h"
#include "vertex_array_object.h"
#include "vertex_buffer_object.h"

class Chunk;

class ChunkRenderer {
 public:
  ChunkRenderer(Chunk* chunk, std::vector<glm::vec3>& vertexPosition,
                const std::vector<glm::vec2>& uvs,
                const std::vector<u32>& indices,
                const std::vector<u8>& directions,
                const std::vector<u8>& light);
  Chunk* chunk;
  void SetWaterMesh(ChunkRenderer* waterMesh);
  void Render();
  void Setup();
  void Destroy();

 private:
  bool ready;
  bool setup;
  std::vector<u32> vertex_position_data;
  std::vector<u32> dir_light_data;
  std::vector<u32> indices;
  ChunkRenderer* water_mesh;
  VertexArrayObject vao;
  VertexBufferObject ibo;
  VertexBufferObject vertex_position_vbo;
  VertexBufferObject dir_light_vbo;
};