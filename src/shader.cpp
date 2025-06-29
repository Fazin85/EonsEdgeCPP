#include "file_util.h"
#include "log.h"
#include "shader.h"
#include "gl_error_check.h"
#include <iostream>
#include <vector>

namespace Eon
{
	Shader::Shader(
		const std::string& vertexShaderName,
		const std::string& fragmentShaderName,
		bool fullPath) :
		vertexName(vertexShaderName),
		fragmentName(fragmentShaderName)
	{
		id = glCreateProgram();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vertexSource = fullPath ? FileUtil::GetFileContents(vertexShaderName) : FileUtil::GetFileContents("content/shaders/" + vertexShaderName);
		std::string fragmentSource = fullPath ? FileUtil::GetFileContents(fragmentShaderName) : FileUtil::GetFileContents("content/shaders/" + fragmentShaderName);

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

		GL_CHECK(glShaderSource(vertexShader, 1, &vertexShaderData, nullptr));
		GL_CHECK(glCompileShader(vertexShader));

		GLint isCompiled = 0;
		GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			GL_CHECK(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> errorLog(maxLength);
			GL_CHECK(glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]));
			EON_INFO("Vertex shader compile error");
			EON_INFO(errorLog.data());

			GL_CHECK(glDeleteShader(vertexShader));
			return;
		}

		GL_CHECK(glShaderSource(fragmentShader, 1, &fragmentShaderData, nullptr));
		GL_CHECK(glCompileShader(fragmentShader));

		isCompiled = 0;
		GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			GL_CHECK(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> errorLog(maxLength);
			GL_CHECK(glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]));
			EON_INFO("Fragment shader compile error");
			EON_INFO(errorLog.data());

			GL_CHECK(glDeleteShader(fragmentShader));
			return;
		}

		GL_CHECK(glAttachShader(id, vertexShader));
		GL_CHECK(glAttachShader(id, fragmentShader));

		GL_CHECK(glLinkProgram(id));

		GLint linkStatus = 0;
		GL_CHECK(glGetProgramiv(id, GL_LINK_STATUS, &linkStatus));
		if (linkStatus == GL_FALSE)
		{
			GLint maxLength = 0;
			GL_CHECK(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength));
			std::vector<GLchar> errorLog(maxLength);
			GL_CHECK(glGetProgramInfoLog(id, maxLength, &maxLength, &errorLog[0]));
			EON_ERROR("Shader linking failed: " + std::string(errorLog.data()));
			GL_CHECK(glDeleteProgram(id));
			id = 0;
			return;
		}

		GL_CHECK(glDeleteShader(vertexShader));
		GL_CHECK(glDeleteShader(fragmentShader));

		GLint count;
		constexpr GLsizei bufSize = 256;//increase this if we have really long uniform names in our shaders
		std::vector<GLchar> name(bufSize);
		GLint length;
		GLint size;
		GLenum type;
		std::vector<std::string> names;

		Bind();

		GL_CHECK(glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count));

		for (GLuint i = 0; i < count; i++)
		{
			GL_CHECK(glGetActiveUniform(id, i, bufSize, &length, &size, &type, name.data()));

			int location = glGetUniformLocation(id, name.data());
			uniform_cache.try_emplace(name.data(), location);
		}

		EON_INFO("Loaded vertex shader: " + vertexShaderName);
		EON_INFO("Loaded fragment shader: " + fragmentShaderName);
	}

	Shader::~Shader()
	{
		GL_CHECK(glDeleteProgram(id));
	}

	Shader::Shader(Shader&& other) noexcept : id(other.id), uniform_cache(other.uniform_cache), vertexName(other.vertexName), fragmentName(other.fragmentName)
	{
		other.id = 0;
		other.uniform_cache.clear();
	}

	void Shader::Bind() const
	{
		GL_CHECK(glUseProgram(id));
	}

	void Shader::Unbind() const
	{
		GL_CHECK(glUseProgram(0));
	}

	void Shader::UniformMatrix4Transpose(const std::string& uniformName, const glm::mat4& matrix)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniformMatrix4fv(location->second, 1, GL_TRUE, &matrix[0][0]));
		}
	}

	void Shader::UniformMatrix4(const std::string& uniformName, const glm::mat4& matrix)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniformMatrix4fv(location->second, 1, GL_FALSE, &matrix[0][0]));
		}
	}

	void Shader::UniformIVec3(const std::string& uniformName, const glm::ivec3& vec)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform3iv(location->second, 1, &vec[0]));
		}
	}

	void Shader::UniformFVec3(const std::string& uniformName, const glm::vec3& vec)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform3fv(location->second, 1, &vec[0]));
		}
	}

	void Shader::UniformFloat(const std::string& uniformName, const float v)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform1f(location->second, v));
		}
	}

	void Shader::UniformFVec4(const std::string& uniformName, glm::vec4 vec)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform4fv(location->second, 1, &vec[0]));
		}
	}
	void Shader::UniformFVec2(const std::string& uniformName, glm::vec2 vec)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform2fv(location->second, 1, &vec[0]));
		}
	}
	void Shader::UniformInt1(const std::string& uniformName, int value)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform1i(location->second, value));
		}
	}

	void Shader::UniformDouble(const std::string& uniformName, double value)
	{
		if (auto location = uniform_cache.find(uniformName); location != uniform_cache.end())
		{
			GL_CHECK(glUniform1dv(location->second, 1, &value));
		}
	}

	const std::string& Shader::GetVertexName() const
	{
		return vertexName;
	}

	const std::string& Shader::GetFragmentName() const
	{
		return fragmentName;
	}
}