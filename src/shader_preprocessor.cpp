#include "shader_preprocessor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "log.h"

namespace Eon
{
	std::string ShaderPreprocessor::Process(const std::string& mainShaderPath)
	{
		Clear();

		try
		{
			ProcessFile(mainShaderPath, GetDirectoryFromPath(mainShaderPath));
		}
		catch (const std::exception& e)
		{
			EON_INFO(e.what());
			throw std::runtime_error("Shader preprocessing failed: " + std::string(e.what()));
		}

		std::string result;
		result.reserve(lines.size() * 50); // Rough estimate to reduce allocations

		for (const auto& shaderLine : lines)
		{
			result += shaderLine.content + "\n";
		}

		return result;
	}

	void ShaderPreprocessor::ProcessFile(const std::string& filepath, const std::string& basePath)
	{
		std::string normalizedPath = std::filesystem::absolute(filepath).string();

		if (processed_files.find(normalizedPath) != processed_files.end())
		{
			return; // Already processed to prevent circular includes
		}

		processed_files.insert(normalizedPath);

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			throw std::runtime_error("Could not open shader file: " + filepath);
		}

		std::string line;
		int lineNumber = 1;

		while (std::getline(file, line))
		{
			ProcessLine(line, normalizedPath, lineNumber, basePath);
			lineNumber++;
		}

		file.close();
	}

	void ShaderPreprocessor::ProcessLine(const std::string& line, const std::string& filepath, int lineNumber, const std::string& basePath)
	{
		std::string trimmedLine = Trim(line);

		if (trimmedLine.find("#include") == 0 && !IsCommentedOut(line, line.find("#include")))
		{
			HandleInclude(line, filepath, lineNumber, basePath);
		}
		else if (trimmedLine.find("#define") == 0 && !IsCommentedOut(line, line.find("#define")))
		{
			HandleDefine(line);
			lines.emplace_back(ExpandDefines(line), filepath, lineNumber);
		}
		else
		{
			lines.emplace_back(ExpandDefines(line), filepath, lineNumber);
		}
	}

	void ShaderPreprocessor::HandleInclude(const std::string& line, const std::string& filepath, int lineNumber, const std::string& basePath)
	{
		// Find the include path between quotes
		size_t firstQuote = line.find('"');
		size_t lastQuote = line.rfind('"');

		if (firstQuote == std::string::npos || lastQuote == std::string::npos || firstQuote == lastQuote)
		{
			// Try angle brackets
			firstQuote = line.find('<');
			lastQuote = line.rfind('>');

			if (firstQuote == std::string::npos || lastQuote == std::string::npos || firstQuote == lastQuote)
			{
				throw std::runtime_error("Invalid #include syntax in " + filepath + " at line " + std::to_string(lineNumber));
			}
		}

		std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
		std::string fullIncludePath = ResolvePath(includePath, basePath);

		// Add a comment line to mark where the include starts (useful for debugging)
		lines.emplace_back("// Begin include: " + includePath, filepath, lineNumber);

		// Recursively process the included file
		ProcessFile(fullIncludePath, GetDirectoryFromPath(fullIncludePath));

		// Add a comment line to mark where the include ends
		lines.emplace_back("// End include: " + includePath, filepath, lineNumber);
	}

	void ShaderPreprocessor::HandleDefine(const std::string& line)
	{
		std::istringstream iss(line);
		std::string defineKeyword, name, value;

		iss >> defineKeyword >> name;

		// Get the rest of the line as the value
		std::getline(iss, value);
		value = Trim(value);

		defines[name] = value;
	}

	std::string ShaderPreprocessor::ExpandDefines(const std::string& line)
	{
		std::string result = line;

		for (const auto& define : defines)
		{
			const std::string& name = define.first;
			const std::string& value = define.second;

			size_t pos = 0;
			while ((pos = result.find(name, pos)) != std::string::npos)
			{
				// Check if it's a whole word (not part of another identifier)
				bool isWholeWord = true;

				if (pos > 0 && (std::isalnum(result[pos - 1]) || result[pos - 1] == '_'))
				{
					isWholeWord = false;
				}

				if (pos + name.length() < result.length() &&
					(std::isalnum(result[pos + name.length()]) || result[pos + name.length()] == '_'))
				{
					isWholeWord = false;
				}

				if (isWholeWord)
				{
					result.replace(pos, name.length(), value);
					pos += value.length();
				}
				else
				{
					pos += name.length();
				}
			}
		}

		return result;
	}

	void ShaderPreprocessor::Clear()
	{
		lines.clear();
		processed_files.clear();
		// Note: We don't clear defines as they might be set externally
	}

	void ShaderPreprocessor::AddDefine(const std::string& name, const std::string& value)
	{
		defines[name] = value;
	}

	void ShaderPreprocessor::RemoveDefine(const std::string& name)
	{
		defines.erase(name);
	}

	ShaderLine ShaderPreprocessor::GetOriginalLine(int compiledLineNumber) const
	{
		if (compiledLineNumber >= 0 && compiledLineNumber < static_cast<int>(lines.size()))
		{
			return lines[compiledLineNumber];
		}
		return ShaderLine("", "", -1);
	}

	std::string ShaderPreprocessor::GetErrorContext(int compiledLineNumber, int contextLines) const
	{
		std::ostringstream context;

		int start = std::max(0, compiledLineNumber - contextLines);
		int end = std::min(static_cast<int>(lines.size()), compiledLineNumber + contextLines + 1);

		for (int i = start; i < end; ++i)
		{
			const ShaderLine& line = lines[i];
			context << (i == compiledLineNumber ? ">>> " : "    ");
			context << i + 1 << ": " << line.content;
			context << " // " << std::filesystem::path(line.original_file).filename().string()
				<< ":" << line.original_line_number;
			context << "\n";
		}

		return context.str();
	}

	void ShaderPreprocessor::PrintLineMapping() const
	{
		for (size_t i = 0; i < lines.size(); ++i)
		{
			const ShaderLine& line = lines[i];
			std::cout << "Line " << (i + 1) << " -> "
				<< std::filesystem::path(line.original_file).filename().string()
				<< ":" << line.original_line_number << "\n";
		}
	}

	// Helper method implementations
	std::string ShaderPreprocessor::GetDirectoryFromPath(const std::string& filepath)
	{
		return std::filesystem::path(filepath).parent_path().string();
	}

	std::string ShaderPreprocessor::ReadFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			throw std::runtime_error("Could not open file: " + filepath);
		}

		std::ostringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	bool ShaderPreprocessor::IsCommentedOut(const std::string& line, size_t pos)
	{
		// Find the beginning of the line
		size_t lineStart = line.rfind('\n', pos);
		if (lineStart == std::string::npos)
		{
			lineStart = 0;
		}
		else
		{
			lineStart++; // Move past the newline
		}

		// Check if there's a // comment before this position on the same line
		size_t commentPos = line.find("//", lineStart);
		return (commentPos != std::string::npos && commentPos < pos);
	}

	std::string ShaderPreprocessor::Trim(const std::string& str)
	{
		size_t first = str.find_first_not_of(' ');
		if (first == std::string::npos)
		{
			return "";
		}

		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	std::string ShaderPreprocessor::ResolvePath(const std::string& includePath, const std::string& basePath)
	{
		std::filesystem::path includeFilePath(includePath);

		if (includeFilePath.is_absolute())
		{
			return includePath;
		}

		// Try relative to the base path first
		std::filesystem::path fullPath = std::filesystem::path(basePath) / includeFilePath;
		if (std::filesystem::exists(fullPath))
		{
			return fullPath.string();
		}

		// If not found, try relative to current working directory
		if (std::filesystem::exists(includePath))
		{
			return std::filesystem::absolute(includePath).string();
		}

		throw std::runtime_error("Could not resolve include path: " + includePath);
	}
}