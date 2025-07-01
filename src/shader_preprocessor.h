#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace Eon
{
	struct ShaderLine
	{
		std::string content;
		std::string original_file;
		int original_line_number;

		ShaderLine(const std::string& content = "", const std::string& file = "", int line = -1)
			: content(content), original_file(file), original_line_number(line)
		{
		}
	};

	class ShaderPreprocessor
	{
	public:
		// Main processing method
		std::string Process(const std::string& mainShaderPath);

		// Utility methods
		void Clear();
		void AddDefine(const std::string& name, const std::string& value = "");
		void RemoveDefine(const std::string& name);

		// Debug/Error reporting helpers
		ShaderLine GetOriginalLine(int compiledLineNumber) const;
		std::string GetErrorContext(int compiledLineNumber, int contextLines = 3) const;
		void PrintLineMapping() const;

		// Getters
		size_t GetLineCount() const { return lines.size(); }
		const std::vector<ShaderLine>& GetLines() const { return lines; }
		const std::unordered_set<std::string>& GetProcessedFiles() const { return processed_files; }

	private:
		std::vector<ShaderLine> lines;
		std::unordered_set<std::string> processed_files;
		std::unordered_map<std::string, std::string> defines;

		// Helper methods
		std::string GetDirectoryFromPath(const std::string& filepath);
		std::string ReadFile(const std::string& filepath);
		bool IsCommentedOut(const std::string& line, size_t pos);
		std::string Trim(const std::string& str);
		std::string ResolvePath(const std::string& includePath, const std::string& basePath);

		// Core processing methods
		void ProcessFile(const std::string& filepath, const std::string& basePath = "");
		void ProcessLine(const std::string& line, const std::string& filepath, int lineNumber, const std::string& basePath);
		void HandleInclude(const std::string& line, const std::string& filepath, int lineNumber, const std::string& basePath);
		void HandleDefine(const std::string& line);
		std::string ExpandDefines(const std::string& line);
	};
}