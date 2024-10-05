#include "chunk_renderer.h"

#include <glad/glad.h>

#include <array>
#include <cmath>

ChunkRenderer::ChunkRenderer(Chunk* chunk,
                             std::vector<glm::vec3>& vertexPositions,
                             const std::vector<glm::vec2>& uvs,
                             const std::vector<u32>& indices,
                             const std::vector<u8>& directions,
                             const std::vector<u8>& light) {
  this->chunk = chunk;
  this->ready = false;
  this->setup = false;
  this->indices = indices;
  this->water_mesh = nullptr;

  vertex_position_data.reserve(vertexPositions.size());
  dir_light_data.reserve(vertexPositions.size());

  for (int i = 0; i < vertexPositions.size(); i++) {
    u8 texX = static_cast<u8>(
        std::floorf(uvs[i].x >= 1.0f ? 255.0f : uvs[i].x * 256.0f));
    u8 texY = static_cast<u8>(
        std::floorf(uvs[i].y >= 1.0f ? 255.0f : uvs[i].y * 256.0f));

    vertex_position_data.push_back((light[i] << 24) | (directions[i] << 16) |
                                   (texX << 8) | (texY));

    u8 px = static_cast<u8>(vertexPositions[i].x);
    u8 pz = static_cast<u8>(vertexPositions[i].z);
    u16 y16 = static_cast<u16>(vertexPositions[i].y);

    u8* yy = reinterpret_cast<u8*>(&y16);

    dir_light_data.push_back((px << 24) | (pz << 16) | (yy[0] << 8) | (yy[1]));
  }
}

void ChunkRenderer::SetWaterMesh(ChunkRenderer* waterMesh) {
  water_mesh = waterMesh;
}

void ChunkRenderer::Render() {
  if (!ready) {
    return;
  }

  vao.Bind();
  ibo.Bind();

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                 GL_UNSIGNED_INT, 0);

  if (water_mesh != nullptr) {
    water_mesh->Render();
  }
}

void ChunkRenderer::Setup() {
  if (ready) {
    return;
  }

  setup = false;

  vao.Init();
  ibo.Init(indices);

  setup = true;

  dir_light_vbo.Init(dir_light_data);
  vao.Link(1, 1, dir_light_vbo, GL_FLOAT, false);

  vertex_position_vbo.Init(vertex_position_data);
  vao.Link(0, 1, vertex_position_vbo, GL_UNSIGNED_INT, true);

  if (water_mesh != nullptr) {
    water_mesh->Setup();
  }

  ready = true;
}

void ChunkRenderer::Destroy() {
  if (setup) {
    vao.Destroy();
    ibo.Destroy();
  }

  vertex_position_vbo.Destroy();
  dir_light_vbo.Destroy();

  if (water_mesh != nullptr) {
    water_mesh->Destroy();
  }
}
