#pragma once

#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>

namespace Eon
{
	class ByteBuffer
	{
	public:
		// Constructor for writing
		ByteBuffer();

		// Constructor for reading
		explicit ByteBuffer(const std::vector<uint8_t>& data);

		// Get raw buffer data
		const std::vector<uint8_t>& GetData() const;

		// Clear the buffer
		void Clear();

		// Buffer size
		size_t Size() const;

		// Writing methods
		void WriteByte(uint8_t value);

		void WriteBytes(const uint8_t* data, size_t length);

		void WriteBool(bool value);

		void WriteInt(int value);

		void WriteFloat(float value);

		void WriteDouble(double value);

		void WriteString(const std::string& value);

		void WriteIVec3(const glm::ivec3& vec);

		// Reading methods
		uint8_t ReadByte();

		void ReadBytes(uint8_t* output, size_t length);

		bool ReadBool();

		int ReadInt();

		float ReadFloat();

		double ReadDouble();

		std::string ReadString();

		glm::ivec3 ReadVector3i();

		// Utility methods
		bool HasRemaining() const;

		size_t Remaining() const;

		void SetReadPosition(size_t position);

		size_t GetReadPosition() const;

	private:
		std::vector<uint8_t> buffer;
		size_t read_position;
		bool is_reading;
	};
}