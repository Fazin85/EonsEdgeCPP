#pragma once

#include "num.h"

#include <vector>

class IndexBufferObject {
  void Init(std::vector<u32>& data);
  void Bind() const;
  void Unbind();
  void Destroy() const;

 private:
  u32 id;
};