#include "opengl_debug.h"
#include <string>
#include "log.h"

namespace Eon
{
	void OpenGLDebugMessageCallback(
		GLenum source,
		GLenum type,
		[[maybe_unused]] GLuint id,
		[[maybe_unused]] GLenum severity,
		[[maybe_unused]] GLsizei length,
		const GLchar* message,
		[[maybe_unused]] const void* userParam)
	{
		std::string sourceStr = [source]()
			{
				switch (source)
				{
				case GL_DEBUG_SOURCE_API: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
				case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
				case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
				case GL_DEBUG_SOURCE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

		std::string typeStr = [type]()
			{
				switch (type)
				{
				case GL_DEBUG_TYPE_ERROR: return "ERROR";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED";
				case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
				case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
				case GL_DEBUG_TYPE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

		if (type == GL_DEBUG_TYPE_ERROR)
		{
			EON_ERROR("OpenGL [" + typeStr + "] " + sourceStr + ": " + message);
			__debugbreak();
		}
		else
		{
			EON_WARN("OpenGL [" + typeStr + "] " + sourceStr + ": " + message);
		}
	}
}
