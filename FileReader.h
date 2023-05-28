#pragma once
#pragma warning(disable : 4996) 
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>


class FileReader
{
private:
	std::ifstream in;
	unsigned long size;

	unsigned long GetSize()//In bytes
	{
		in.seekg(0, std::ios::end);
		unsigned long size = in.tellg();
		in.seekg(0, std::ios::beg);
		return size;
	}

public:
	unsigned int NumBytes[256] = { 0 };
	double PercBytes[256] = { 0 };

	FileReader(std::string path)
	{
		in.open(path.c_str(), std::ifstream::binary);
		if (!in.is_open())
			throw "Can't open this file\nCheck correctness of the path";
		size = GetSize();
		CalculateBytes();
		CalculatePercents();
	}
	FileReader(){}

	void CalculateBytes()
	{
		size_t temp2 = 1;
		char temp = 0;
		for(unsigned long i = 0; i < size; i++)
		{
			in.read(&temp, 1);
			(*(NumBytes + (unsigned char)temp))++;
		}
	}

	void CalculatePercents()
	{
		for (short i = 0; i < 256; i++)
		{
			*(PercBytes + i) = *(NumBytes + i) / (double)size;
		}
	}

	void Print()
	{
		for (size_t i = 0; i < 256; i++)
		{
			std::cout << std::hex << i << std::dec << "\t" << *(NumBytes + i) << std::endl;
		}
	}

	void Close()
	{
		_fcloseall();
	}
};

