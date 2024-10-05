#include "vertex_array_object.h"

#include "glad/glad.h"

void VertexArrayObject::Init() {
  glGenVertexArrays(1, &id);

  initialized = true;
}

void VertexArrayObject::Link(int location, int size, VertexBufferObject& vbo,
                             u32 type, bool integer) {
  Bind();
  vbo.Bind();
  if (integer) {
    glVertexAttribIPointer(location, size, type, 0, 0);
  } else {
    glVertexAttribPointer(location, size, type, false, 0, 0);
  }
  glEnableVertexAttribArray(location);
  Unbind();
}

void VertexArrayObject::Bind() const { glBindVertexArray(id); }

void VertexArrayObject::Unbind() { glBindVertexArray(0); }

void VertexArrayObject::Destroy() {
  glDeleteVertexArrays(1, &id);
  initialized = false;
}

bool VertexArrayObject::Initialized() const { return initialized; }
