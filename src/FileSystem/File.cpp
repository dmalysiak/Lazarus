/*
 * File.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: Darius Malysiak
 */

#include "File.h"
#include "../Toolbox.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace Lazarus {

File::File(const std::string& file) {

	mp_data = NULL;
	m_data_size = 0;
	m_is_mapped = false;
	m_filename = file;

}

File::~File() {

	//in case the file has been mapped: unmap
	if(m_is_mapped == true)
	{
		munmap( mp_data, m_data_size);
	}
	else
	{
		DELETE_NULL_CHECKING(mp_data);
	}

}

unsigned long File::getSize()
{
	return m_data_size;
}

unsigned char* File::getData()
{
	return mp_data;
}

int File::mapFile(int flags, unsigned long offset, unsigned long size)
{
	int ret = 0;

	//if file already has been mapped:unmap and delete any loaded data in the other case
	if(m_is_mapped == true)
	{
		munmap( mp_data, m_data_size);
		m_is_mapped = false;
	}
	else
	{
		DELETE_NULL_CHECKING(mp_data);
	}

	//check if file exists, if not then create a new file
	if(Lazarus::Toolbox::checkFileExistence(m_filename) == false)
	{
		Lazarus::Toolbox::createEmptyFile(m_filename,offset+size,"1");
		ret = -2;
	}

	//open the file and determine its length
	unsigned long filesize = 0;
	FILE* fp = fopen(m_filename.c_str(),"r");

	if(fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return -1;
	}

	filesize = Toolbox::getFileSize(fp);
	//fread(&filesize,sizeof(unsigned int),1,fp);

	fclose(fp);

	//now open it through a direct system call (an alternative would be to catch the return value of 'fileno(fp)')
	int filedes = open(m_filename.c_str(),O_RDWR);

	//check offset for sanity
	if(offset >= filesize)
	{
		printf("ERROR: offset to large\n");
		close(filedes);
		return -1;
	}

	//map the file
	if(size != 0 && size <= filesize-offset)//in case a size was specified which does not exceed the amount of available data
	{
		mp_data = (unsigned char*)mmap( NULL, size, PROT_READ | PROT_WRITE, flags, filedes, offset);
		m_data_size = size;
	}
	else if(size != 0 && size > filesize-offset) //if a size was specified yet exceeds the amount available data
	{
		//print warning but read all data until the files end
		printf("WARNING: given size exceeds file size\n");
		mp_data = (unsigned char*)mmap( NULL, filesize-offset, PROT_READ | PROT_WRITE, flags, filedes, offset);
		m_data_size = filesize-offset;
	}
	else //map all data from the given offset until the files' end
	{
		mp_data = (unsigned char*)mmap( NULL, filesize-offset, PROT_READ | PROT_WRITE, flags, filedes, offset);
		m_data_size = filesize-offset;
	}

	close(filedes);
	m_is_mapped = true;

	return ret;
}

int File::readFile(unsigned long offset, unsigned long size)
{
	int ret = 0;

	//in case the file has been mapped: unmap
	if(m_is_mapped == true)
	{
		munmap( mp_data, m_data_size);
		m_is_mapped = false;
	}

	//check if file exists, if not then create a new file
	if(Lazarus::Toolbox::checkFileExistence(m_filename) == false)
	{
		Lazarus::Toolbox::createEmptyFile(m_filename,offset+size,"1");
		ret = -2;
	}

	//open the file and determine its length
	FILE* fp = fopen(m_filename.c_str(),"r");

	if(fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return -1;
	}

	m_data_size = Toolbox::getFileSize(fp);
	//fread(&m_data_size,sizeof(unsigned int),1,fp);

	//check offset for sanity
	if(offset >= m_data_size)
	{
		printf("ERROR: offset to large\n");
		fclose(fp);
		return -1;
	}

	//allocate memory and read all data
	DELETE_ARRAY_NULL_CHECKING(mp_data);

	mp_data = new unsigned char[m_data_size];

	if(size != 0 && size <= m_data_size-offset)//in case a size was specified
	{
		//set the offset
		fseek(fp, offset ,SEEK_SET);
		fread(mp_data,sizeof(unsigned char),size,fp);
		m_data_size = size;
	}
	else if(size != 0 && size > m_data_size-offset) //if a size was specified yet exceeds the amount available data
	{
		//print warning but read all data until the files end
		printf("WARNING: given size exceeds file size\n");
		//set the offset
		fseek(fp, offset ,SEEK_SET);
		fread(mp_data,sizeof(unsigned char),m_data_size,fp);
		m_data_size = size-offset;
	}
	else// read all data
	{
		//set the offset
		fseek(fp, offset ,SEEK_SET);
		fread(mp_data,sizeof(unsigned char),m_data_size,fp);
	}

	fclose(fp);

	return ret;
}

int File::writeFile()
{
	if(m_is_mapped == true)
	{
		printf("ERROR: write impossible; file is mapped \n");
		return -1;
	}

	FILE* fp = fopen(m_filename.c_str(),"wb");

	if(fp == NULL)
	{
		printf("ERROR: could not open file\n");
		return -1;
	}

	//write the content
	fwrite(mp_data,sizeof(unsigned char),m_data_size,fp);

	fclose(fp);

	return 0;
}

void File::assignData(unsigned char* data, unsigned long size)
{
	//abort in case of existing mapping
	if(m_is_mapped == true)
	{
		printf("ERROR: can not update memory for a mapped file");

		return;
	}

	DELETE_ARRAY_NULL_CHECKING(mp_data);

	mp_data = data;
	m_data_size = size;
}

int File::syncMap()
{
	//a synchronized write
	int res = msync(mp_data, m_data_size, MS_SYNC);

	return res;
}

} /* namespace Lazarus */
