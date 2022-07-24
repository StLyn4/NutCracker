
#pragma once

#include <iostream>
#include <algorithm>
#include <stdint.h>
#include "Errors.h"

enum ArchType
{
	UNKNOWN,
	X32,
	X64,
};

// ************************************************************************************************************************************
class BinaryReader
{
private:
	std::istream& _in;

	// Delete default methods
	BinaryReader();
	BinaryReader( const BinaryReader& );
	BinaryReader& operator=( const BinaryReader& );

public:
	ArchType targetArch;

	explicit BinaryReader( std::istream& in )
	: _in(in)
	{
		targetArch = UNKNOWN;
	}

	// ******************************************************************************
	ArchType CheckArch( void )
	{
		std::streamoff currentPos = _in.tellg();

		_in.seekg(2, _in.beg);
		if (ReadInt32() == 'SQIR')
		{
			targetArch = X32;
		}

		_in.seekg(2, _in.beg);
		if (ReadInt64() == 'SQIR')
		{
			targetArch = X64;
		}

		_in.seekg(currentPos, _in.beg);
		return targetArch;
	}

	// ******************************************************************************
	uint64_t ReadArchUInt( void )
	{
		return targetArch == X32 ? ReadUInt32() : ReadUInt64();
	}

	// ******************************************************************************
	int64_t ReadArchInt( void )
	{
		return targetArch == X32 ? ReadInt32() : ReadInt64();
	}

	// ******************************************************************************
	uint32_t ReadUInt32( void )
	{
		uint32_t value;

		//static_assert(sizeof(value) == 4, "Invalid size of uint32_t type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}

	// ******************************************************************************
	int32_t ReadInt32( void )
	{
		int32_t value;

		//static_assert(sizeof(value) == 4, "Invalid size of int32_t type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}

	// ******************************************************************************
	uint64_t ReadUInt64( void )
	{
		uint64_t value;

		//static_assert(sizeof(value) == 4, "Invalid size of uint64_t type");

		_in.read((char*)&value, 8);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}

	// ******************************************************************************
	int64_t ReadInt64( void )
	{
		int64_t value;

		//static_assert(sizeof(value) == 4, "Invalid size of int64_t type");

		_in.read((char*)&value, 8);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	uint16_t ReadUInt16( void )
	{
		uint16_t value;

		//static_assert(sizeof(value) == 2, "Invalid size of uint16_t");

		_in.read((char*)&value, 2);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	int16_t ReadInt16( void )
	{
		int16_t value;

		//static_assert(sizeof(value) == 2, "Invalid size of int16_t");

		_in.read((char*)&value, 2);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	int8_t ReadByte( void )
	{
		int8_t value;

		//static_assert(sizeof(value) == 1, "Invalid size of char type");

		_in.read((char*)&value, 1);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	float ReadFloat32( void )
	{
		float value;

		//static_assert(sizeof(value) == 4, "Invalid size of float type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	double ReadFloat64( void )
	{
		double value;

		//static_assert(sizeof(value) == 8, "Invalid size of double type");

		_in.read((char*)&value, 8);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	bool ReadBool( void )
	{
		bool value;

		_in.read((char*)&value, sizeof(bool));

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	void Read( void* buffer, uint64_t size )
	{
		if (size < 1)
			return;

		_in.read((char*)buffer, size);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");
	}

	std::streampos position() {
		return _in.tellg();
	}

	// ******************************************************************************
	void ConfirmOnPart( void )
	{
		int64_t data = ReadArchInt();

		if (data != 'PART')
			throw Error("Bad format of source binary file (PART marker was not match).");
	}


	// ******************************************************************************
	void ReadSQString( std::string& str )
	{
		int64_t len = ReadArchInt();
		str.clear();
		str.reserve((size_t) len);

		while(len > 0)
		{
			char buffer[128];
			int64_t chunk = std::min((int64_t) 128, len);

			Read(buffer, chunk);

			str.append(buffer, (size_t) chunk);
			len -= chunk;
		}
	}


	// ******************************************************************************
	void ReadSQStringObject( std::string& str )
	{
		static const int StringObjectType = 0x10 | 0x08000000;
		static const int NullObjectType = 0x01 | 0x01000000;

		int32_t type = ReadInt32();

		if (type == StringObjectType)
			ReadSQString(str);
		else if (type == NullObjectType)
			str.clear();
		else
			throw Error("Expected string object not found in source binary file.");
	}
};
