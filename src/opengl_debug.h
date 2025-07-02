#pragma once

#include "glad/glad.h"

namespace Eon
{
	void APIENTRY OpenGLDebugMessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
}