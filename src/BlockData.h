#pragma once
#include <cstdint>

//Struct for BlockData
//Occupies 32 bits (4 bytes)
//16 bit BlockID
//5 bit lighting
//5 bit sunlight

struct BlockData
{
	uint32_t data;
	BlockData()
	{

	}
	BlockData(uint32_t data)
	{
		this->data = data;
	}
	BlockData(short blockID, short light, short sunlight)
	{
		data = (blockID | (light << 16)) | (sunlight << 21);
	}
	short getBlockID() const
	{
		return (data & 0b00000000000000001111111111111111);
	}
	short getLightLevel() const
	{
		return (data & 0b00000000000111110000000000000000) >> 16;
	}
	short getSunLightLevel() const
	{
		return (data & 0b00000011111000000000000000000000) >> 21;
	}
	//Operator overloading for ease of use
	BlockData& operator=(const int& rhs)
	{
		this->data = rhs;
		return *this;
	}

	bool operator==(const int& rhs)
	{
		return (getBlockID() == rhs);
	}
};

