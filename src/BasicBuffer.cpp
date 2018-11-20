/*
 * Buffer.cpp
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#include "BasicBuffer.h"
#include "Toolbox.h"

#include <stdio.h>
#include <string.h>

namespace Lazarus
{

BasicBuffer::BasicBuffer()
{
	m_length = 0;
	mp_data = NULL;
}

BasicBuffer::BasicBuffer(unsigned int length)
{
	m_length = length;
	mp_data = new unsigned char[length];
}

BasicBuffer::~BasicBuffer()
{
	DELETE_ARRAY_NULL_CHECKING(mp_data);
}

void BasicBuffer::changeSize(unsigned int length)
{
	unsigned char* new_data = new unsigned char[length];
	unsigned int min_length = std::min(length,m_length);

	if(mp_data != NULL)
	{
		memcpy(new_data, mp_data, min_length);
	}

	DELETE_ARRAY_NULL_CHECKING(mp_data);

	m_length = length;
	mp_data = new_data;
}

void BasicBuffer::changeSizeZero(unsigned int length)
{
	unsigned char* new_data = new unsigned char[length];
	memset(new_data,0,length);

	unsigned int min_length = std::min(length,m_length);

	if(mp_data != NULL)
	{
		memcpy(new_data, mp_data, min_length);
	}

	DELETE_ARRAY_NULL_CHECKING(mp_data);

	m_length = length;
	mp_data = new_data;
}

unsigned int BasicBuffer::get_m_length() const
{
	return m_length;
}

unsigned char* BasicBuffer::get_mp_data() const
{
	return mp_data;
}


void BasicBuffer::printfBuffer(enum BUFFER_DATA_TYPE type) const
{
	printf("Buffersize is %d\n",m_length);

	switch(type)
	{
		case BUFFER_DATA_TYPE_UCHAR:
		{
			printf("Printing buffer values as unsigned char:\n");
			for(unsigned int i = 0;i<m_length;i++)
			{
				printf("%u ",mp_data[i]);
			}

			break;
		}

		case BUFFER_DATA_TYPE_FLOAT:
		{
			printf("Printing buffer values as float:\n");
			for(unsigned int i = 0;i < m_length/sizeof(float);i++)
			{
				printf("%f ",((float*)mp_data)[i]);
			}

			break;
		}

		case BUFFER_DATA_TYPE_DOUBLE:
		{
			printf("Printing buffer values as double:\n");
			for(unsigned int i = 0;i < m_length/sizeof(double);i++)
			{
				printf("%F ",((double*)mp_data)[i]);
			}

			break;
		}

		case BUFFER_DATA_TYPE_UINT:
		{
			printf("Printing buffer values as double:\n");
			for(unsigned int i = 0;i < m_length/sizeof(unsigned int);i++)
			{
				printf("%u ",((unsigned int*)mp_data)[i]);
			}

			break;
		}

		default:
			printf("ERROR: something went wrong during buffer printing");
			break;
	}



	printf("\n");

}



void BasicBuffer::printfBufferChar() const
{
	printf("Buffersize is %d\n",m_length);

	printf("Printing buffer values as unsigned char:\n");
	for(unsigned int i = 0;i<m_length;i++)
	{
		printf("%c",mp_data[i]);
	}

	printf("\n");

}


void BasicBuffer::printfRawBuffer() const
{
	printf("Buffersize is %d\n",m_length);

	for(unsigned int i = 0;i<m_length;i++)
	{
		printf("%d ",mp_data[i]);
	}

	printf("\n");

}

void BasicBuffer::printfRawBufferHex() const
{
	printf("Buffersize is %d\n",m_length);

	for(unsigned int i = 0;i<m_length;i++)
	{
		printf("%02x ",mp_data[i]);
	}

	printf("\n");

}

void BasicBuffer::writeBufferToBinaryFile(const std::string& filename) const
{

	FILE* fp = fopen(filename.c_str(),"wb");

	if(fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return;
	}

	//write the content
	fwrite(mp_data,sizeof(unsigned char),m_length,fp);

	fclose(fp);

}

void BasicBuffer::readBufferFromBinaryFile(const std::string& filename)
{
	FILE* fp = fopen(filename.c_str(),"rb");

	if(fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return;
	}

	//read the content
	m_length = Toolbox::getFileSize(fp);

	DELETE_ARRAY_NULL_CHECKING(mp_data);

	mp_data = new unsigned char[m_length];

	fread(mp_data,sizeof(unsigned char),m_length,fp);

	fclose(fp);
}

void BasicBuffer::writeBufferToTextFile(const std::string& filename) const
{
	Toolbox::writeArrayToTextFile(filename,mp_data,m_length);
}

void BasicBuffer::readBufferFromTextFile(const std::string& filename)
{
	FILE* fp;
	fp = fopen(filename.c_str(),"r");
	if (fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return;
	}

	//read the content
	m_length = Toolbox::getFileSize(fp);
	fclose(fp);

	DELETE_ARRAY_NULL_CHECKING(mp_data);

	mp_data = new unsigned char[m_length];

	int res = Lazarus::Toolbox::readTextFileToArray(filename,mp_data);

	if(res ==-1)
	{
		printf("ERROR: could not read txt file into buffer.\n");
	}

}

}
