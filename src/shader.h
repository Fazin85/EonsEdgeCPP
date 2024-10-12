#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>
#include <unordered_map>

namespace Eon
{
	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
		~Shader();

		void Bind() const;
		void Unbind();

		void UniformMatrix4(const std::string& uniformName, const glm::mat4& matrix);
		void UniformIVec3(const std::string& uniformName, const glm::ivec3& vec);
		void UniformFVec3(const std::string& uniformName, const glm::vec3& vec);
		void UniformFloat(const std::string& uniformName, const float v);
		void UniformFVec4(const std::string& uniformName, glm::vec4 vec);

	private:
		GLuint id;
		std::unordered_map<std::string, GLint> uniform_cache;
	};
}