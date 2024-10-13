#include "file_util.h"
#include "log.h"
#include "shader.h"
#include <iostream>
#include <vector>
#include "SFML/OpenGL.hpp"

namespace Eon
{
	Shader::Shader(const std::string& vertexShaderName, const std::string& fragmentShaderName)
	{
		id = glCreateProgram();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertexSource = FileUtil::GetFileContents("Content/Shaders/" + vertexShaderName);
		std::string fragmentSource = FileUtil::GetFileContents("Content/Shaders/" + fragmentShaderName);

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

		glShaderSource(fragmentShader, 1, &fragmentShaderData, nullptr);
		glCompileShader(fragmentShader);

		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);

		glLinkProgram(id);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint count;
		constexpr GLsizei bufSize = 256;//increase this if we have really long uniform names in our shaders
		GLchar name[bufSize];
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
			uniform_cache.insert({ name, location });
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

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::UniformMatrix4(const std::string& uniformName, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(uniform_cache[uniformName], 1, GL_TRUE, &matrix[0][0]);
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
}