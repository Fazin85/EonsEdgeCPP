#include "byte_buffer.h"

#include <array>

namespace Eon
{
	ByteBuffer::ByteBuffer() : read_position(0), is_reading(false) {}

	// Constructor for reading
	ByteBuffer::ByteBuffer(const std::vector<uint8_t>& data) : buffer(data), read_position(0), is_reading(true) {}

	// Get raw buffer data
	const std::vector<uint8_t>& ByteBuffer::GetData() const
	{
		return buffer;
	}

	// Clear the buffer
	void ByteBuffer::Clear()
	{
		buffer.clear();
		read_position = 0;
	}

	// Buffer size
	size_t ByteBuffer::Size() const
	{
		return buffer.size();
	}

	// Writing methods
	void ByteBuffer::WriteByte(uint8_t value)
	{
		buffer.push_back(value);
	}

	void ByteBuffer::WriteBytes(const uint8_t* data, size_t length)
	{
		buffer.insert(buffer.end(), data, data + length);
	}

	void ByteBuffer::WriteBool(bool value)
	{
		WriteByte(value ? 1 : 0);
	}

	void ByteBuffer::WriteInt(int value)
	{
		std::array<uint8_t, 4> bytes = { 0 };
		bytes[0] = (value >> 24) & 0xFF;
		bytes[1] = (value >> 16) & 0xFF;
		bytes[2] = (value >> 8) & 0xFF;
		bytes[3] = value & 0xFF;
		WriteBytes(bytes.data(), 4);
	}

	void ByteBuffer::WriteFloat(float value)
	{
		std::array<uint8_t, 4> bytes = { 0 };
		std::memcpy(bytes.data(), &value, 4);
		WriteBytes(bytes.data(), 4);
	}

	void ByteBuffer::WriteDouble(double value)
	{
		std::array<uint8_t, 8> bytes = { 0 };
		std::memcpy(bytes.data(), &value, 8);
		WriteBytes(bytes.data(), 8);
	}

	void ByteBuffer::WriteString(const std::string& value)
	{
		// Write string length first
		WriteInt(static_cast<int>(value.length()));
		// Then write string data
		WriteBytes(reinterpret_cast<const uint8_t*>(value.c_str()), value.length());
	}

	void ByteBuffer::WriteIVec3(const glm::ivec3& vec)
	{
		WriteInt(vec.x);
		WriteInt(vec.y);
		WriteInt(vec.z);
	}

	// Reading methods
	uint8_t ByteBuffer::ReadByte()
	{
		if (read_position >= buffer.size())
		{
			throw std::out_of_range("Buffer underflow");
		}
		return buffer[read_position++];
	}

	void ByteBuffer::ReadBytes(uint8_t* output, size_t length)
	{
		if (read_position + length > buffer.size())
		{
			throw std::out_of_range("Buffer underflow");
		}
		std::memcpy(output, buffer.data() + read_position, length);
		read_position += length;
	}

	bool ByteBuffer::ReadBool()
	{
		return ReadByte() != 0;
	}

	int ByteBuffer::ReadInt()
	{
		if (read_position + 4 > buffer.size())
		{
			throw std::out_of_range("Buffer underflow");
		}

		int value = 0;
		value |= static_cast<int>(buffer[read_position++]) << 24;
		value |= static_cast<int>(buffer[read_position++]) << 16;
		value |= static_cast<int>(buffer[read_position++]) << 8;
		value |= static_cast<int>(buffer[read_position++]);
		return value;
	}

	float ByteBuffer::ReadFloat()
	{
		if (read_position + 4 > buffer.size())
		{
			throw std::out_of_range("Buffer underflow");
		}

		float value;
		std::memcpy(&value, buffer.data() + read_position, 4);
		read_position += 4;
		return value;
	}

	double ByteBuffer::ReadDouble()
	{
		if (read_position + 8 > buffer.size())
		{
			throw std::out_of_range("Buffer underflow");
		}

		double value;
		std::memcpy(&value, buffer.data() + read_position, 8);
		read_position += 8;
		return value;
	}

	std::string ByteBuffer::ReadString()
	{
		int length = ReadInt();
		if (length < 0 || read_position + length > buffer.size())
		{
			throw std::out_of_range("Buffer underflow or invalid string length");
		}

		std::string value(reinterpret_cast<char*>(buffer.data() + read_position), length);
		read_position += length;
		return value;
	}

	glm::ivec3 ByteBuffer::ReadVector3i()
	{
		glm::ivec3 vec;
		vec.x = ReadInt();
		vec.y = ReadInt();
		vec.z = ReadInt();
		return vec;
	}

	// Utility methods
	bool ByteBuffer::HasRemaining() const
	{
		return read_position < buffer.size();
	}

	size_t ByteBuffer::Remaining() const
	{
		return buffer.size() - read_position;
	}

	void ByteBuffer::SetReadPosition(size_t position)
	{
		if (position > buffer.size())
		{
			throw std::out_of_range("Invalid read position");
		}
		read_position = position;
	}

	size_t ByteBuffer::GetReadPosition() const
	{
		return read_position;
	}
}