#include "gl_error_check.h"
#include "log.h"

namespace Eon
{
	std::string GLErrorToString(GLenum error)
	{
		switch (error)
		{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_CONTEXT_LOST:
			return "GL_CONTEXT_LOST";
		default:
			return "UNKNOWN_GL_ERROR (" + std::to_string(error) + ")";
		}
	}

	bool CheckGLError(const std::string& operation)
	{
		GLenum error = glGetError();
		bool hasError = false;

		while (error != GL_NO_ERROR)
		{
			hasError = true;
			std::string errorMsg = "OpenGL Error: " + GLErrorToString(error);

			if (!operation.empty())
			{
				errorMsg += " during operation: " + operation;
			}

			EON_ERROR(errorMsg);
			error = glGetError();
		}

		return !hasError;
	}

	void ClearGLErrors()
	{
		while (glGetError() != GL_NO_ERROR)
		{
		}
	}

	bool CheckGLErrorDetailed(const char* file, int line, const std::string& operation)
	{
		GLenum error = glGetError();
		bool hasError = false;

		while (error != GL_NO_ERROR)
		{
			hasError = true;
			std::string errorMsg = "OpenGL Error: " + GLErrorToString(error);

			if (!operation.empty())
			{
				errorMsg += " during operation: " + operation;
			}

			errorMsg += " at " + std::string(file) + ":" + std::to_string(line);

			EON_ERROR(errorMsg);
			error = glGetError();
		}

		return !hasError;
	}
}