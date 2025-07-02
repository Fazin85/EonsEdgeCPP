#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace Eon
{
	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, bool fullPath = false);
		~Shader();

		Shader(Shader&& other) noexcept;
		
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		void Bind() const;
		void Unbind() const;

		void UniformMatrix4Transpose(const std::string& uniformName, const glm::mat4& matrix);
		void UniformMatrix4(const std::string& uniformName, const glm::mat4& matrix);
		void UniformIVec3(const std::string& uniformName, const glm::ivec3& vec);
		void UniformFVec3(const std::string& uniformName, const glm::vec3& vec);
		void UniformFloat(const std::string& uniformName, const float v);
		void UniformFVec4(const std::string& uniformName, glm::vec4 vec);
		void UniformFVec2(const std::string& uniformName, glm::vec2 vec);
		void UniformInt1(const std::string& uniformName, int value);
		void UniformDouble(const std::string& uniformName, double value);

		const std::string& GetVertexName() const;
		const std::string& GetFragmentName() const;

	private:
		GLuint id;
		std::unordered_map<std::string, GLint> uniform_cache;
		const std::string vertexName;
		const std::string fragmentName;
	};
}