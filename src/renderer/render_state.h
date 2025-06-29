#pragma once

#include <glad/glad.h>

namespace Eon
{
	struct RenderState
	{
		GLenum depth_function = GL_LESS;
		bool depth_test = true;
		bool blend = false;
		GLenum blend_src = GL_SRC_ALPHA;
		GLenum blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		bool cull_face = true;
		GLenum cull_face_mode = GL_BACK;

		void Apply() const;
		void Reset();
	};
}