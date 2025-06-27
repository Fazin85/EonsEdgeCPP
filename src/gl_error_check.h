#pragma once

#include <glad/glad.h>
#include <string>

namespace Eon
{
	bool CheckGLError(const std::string& operation = "");

	void ClearGLErrors();

	bool CheckGLErrorDetailed(const char* file, int line, const std::string& operation = "");

	std::string GLErrorToString(GLenum error);
}

#ifdef _DEBUG
#define GL_CHECK(operation) \
    do { \
        operation; \
        Eon::CheckGLErrorDetailed(__FILE__, __LINE__, #operation); \
    } while(0)

#define GL_CHECK_ERRORS() \
    Eon::CheckGLErrorDetailed(__FILE__, __LINE__)    
#else
#define GL_CHECK(operation) operation
#define GL_CHECK_ERRORS()
#endif