#include "file_util.h"
#include "log.h"
#include "shader.h"
#include <iostream>
#include <vector>

namespace Eon
{
	Shader::Shader(const std::string& vertexShaderName, const std::string& fragmentShaderName)
	{
		id = glCreateProgram();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertexSource = FileUtil::GetFileContents("content/shaders/" + vertexShaderName);
		std::string fragmentSource = FileUtil::GetFileContents("content/shaders/" + fragmentShaderName);

		if (vertexSource == "")
		{
			EON_ERROR("Failed to load vertex shader: " + vertexShaderName);
		}

		if (fragmentSource == "")
		{
			EON_ERROR("Failed to load fragment shader: " + fragmentShaderName);
		}

		const char* vertexShaderData = vertexSource.c_str();
		const char* fragmentShaderData = fragmentSource.c_str();

		glShaderSource(vertexShader, 1, &vertexShaderData, nullptr);
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);
			EON_INFO(errorLog.data());

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(vertexShader); // Don't leak the shader.
			return;
		}

		glShaderSource(fragmentShader, 1, &fragmentShaderData, nullptr);
		glCompileShader(fragmentShader);

		isCompiled = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);
			EON_INFO(errorLog.data());

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(fragmentShader); // Don't leak the shader.
			return;
		}

		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);

		glLinkProgram(id);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint count;
		constexpr GLsizei bufSize = 256;//increase this if we have really long uniform names in our shaders
		GLchar name[bufSize] = { 0 };
		GLint length;
		GLint size;
		GLenum type;
		std::vector<std::string> names;

		Bind();

		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

		for (GLuint i = 0; i < count; i++)
		{
			glGetActiveUniform(id, i, bufSize, &length, &size, &type, name);

			int location = glGetUniformLocation(id, name);
			uniform_cache.try_emplace(name, location);
		}

		EON_INFO("Loaded vertex shader: " + vertexShaderName);
		EON_INFO("Loaded fragment shader: " + fragmentShaderName);
	}

	Shader::~Shader()
	{
		glDeleteProgram(id);
	}

	void Shader::Bind() const
	{
		glUseProgram(id);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::UniformMatrix4(const std::string& uniformName, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(uniform_cache[uniformName], 1, GL_TRUE, &matrix[0][0]);
	}

	void Shader::UniformMatrix4NoTranspose(const std::string& uniformName, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(uniform_cache[uniformName], 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::UniformIVec3(const std::string& uniformName, const glm::ivec3& vec)
	{
		glUniform3iv(uniform_cache[uniformName], 1, &vec[0]);
	}

	void Shader::UniformFVec3(const std::string& uniformName, const glm::vec3& vec)
	{
		glUniform3fv(uniform_cache[uniformName], 1, &vec[0]);
	}

	void Shader::UniformFloat(const std::string& uniformName, const float v)
	{
		glUniform1f(uniform_cache[uniformName], v);
	}

	void Shader::UniformFVec4(const std::string& uniformName, glm::vec4 vec)
	{
		glUniform4fv(uniform_cache[uniformName], 1, &vec[0]);
	}
	void Shader::UniformFVec2(const std::string& uniformName, glm::vec2 vec)
	{
		glUniform2fv(uniform_cache[uniformName], 1, &vec[0]);
	}
	void Shader::UniformInt1(const std::string& uniformName, int value)
	{
		glUniform1i(uniform_cache[uniformName], value);
	}
}