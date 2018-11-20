/*
 * Camera.cpp
 *
 *  Created on: Sep 27, 2013
 *      Author: clustro
 */

#include "WebCamera.h"
#include "../../ImageProcessing/ImageProcessingToolbox.h"
#include "../../ImageProcessing/JPEGCompressionBox.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace Lazarus {

WebCamera::WebCamera()
{

	m_fd = -1;
	mp_buffer = NULL;
	m_mapped = false;
	m_buffer_length = 0;
	m_width = 0;
	m_height = 0;
	m_image = NULL;
	m_isStreaming = false;
}

WebCamera::~WebCamera() {
	closeDevice();
	DELETE_NULL_CHECKING(m_image);
}


unsigned char* WebCamera::getFrame()
{
	return NULL;
}

void WebCamera::reset()
{
    printf("reset !\n");
    uninit_mmap();
    closeDevice();

    openDevice(m_dev);
    init_mmap();
}

int WebCamera::xioctl(int fh, int request, void *arg)
{
	int r;

	do
	{
		r = ioctl(fh, request, arg);
	}while(r == -1 && errno == EINTR);

	return r;
}

int WebCamera::init_mmap()
{
	struct v4l2_requestbuffers req = {0};
	req.count = 1;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if(xioctl(m_fd, VIDIOC_REQBUFS, &req) == -1)
	{
		perror("Requesting Buffer");
		return 1;
	}

	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if(xioctl(m_fd, VIDIOC_QUERYBUF, &buf) == -1)
	{
		perror("Querying Buffer");
		return 1;
	}

	mp_buffer = (unsigned char*)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buf.m.offset);
	printf("Length: %d Address: %p\n", buf.length, mp_buffer);
	printf("Image Length: %d\n", buf.bytesused);

	m_mapped = true;
	m_buffer_length =  buf.length;

	return 0;
}

int WebCamera::startStream()
{
	//in case that the memory hasn't been mapped yet; map it
	if(m_mapped == false)
	{
		//map the device memory into our local memory
		init_mmap();
	}

	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if(xioctl(m_fd, VIDIOC_STREAMON, &buf.type) == -1)
	{
		perror("Start Capture");
		return -1;
	}

	m_isStreaming = true;

	return 0;
}

int WebCamera::stopStream()
{
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if(xioctl(m_fd, VIDIOC_STREAMOFF, &buf.type) == -1) {
	    perror("Stop Capture");
	    return -1;
	}

	m_isStreaming = false;

	return 0;
}

Lazarus::Image<unsigned char>* WebCamera::capture_image()
{
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if(xioctl(m_fd, VIDIOC_QBUF, &buf) == -1)
	{
		perror("Query Buffer");
		return m_image;
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_fd, &fds);

	struct timeval tv = {0};
	tv.tv_sec = 2;

	int r = select(m_fd+1, &fds, NULL, NULL, &tv);

	if(r == -1)
	{
		perror("Waiting for Frame timeout");
		reset();
		return m_image;
	}

	if(xioctl(m_fd, VIDIOC_DQBUF, &buf) == -1)
	{
		perror("Retrieving Frame");
		return m_image;
	}

	//image should be in mp_buffer now
	//printf("frame retrieved\n");
	//printf("Image Length: %d\n", buf.bytesused);

	//ImageProcessingToolbox::convertYUV422ToRGBSequential((char*)mp_buffer,image->getmp_image_data(),m_width,m_height);
	//ImageProcessingToolbox::convertYUV422ToRGB((char*)mp_buffer,m_image->getmp_image_data(),m_width,m_height);

	PrimitiveBuffer buff(mp_buffer,buf.bytesused);
	JPEGCompressionBox<unsigned char>::decompress( &buff , m_image);

	return m_image;
}


Lazarus::PrimitiveBuffer WebCamera::capture_image_buffer()
{
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if(xioctl(m_fd, VIDIOC_QBUF, &buf) == -1)
	{
		perror("Query Buffer");
		return Lazarus::PrimitiveBuffer();
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_fd, &fds);

	struct timeval tv = {0};
	tv.tv_sec = 2;

	int r = select(m_fd+1, &fds, NULL, NULL, &tv);

	if(r == -1)
	{
		perror("Waiting for Frame timeout");
		reset();
		return Lazarus::PrimitiveBuffer();
	}

	if(xioctl(m_fd, VIDIOC_DQBUF, &buf) == -1)
	{
		perror("Retrieving Frame");
		return Lazarus::PrimitiveBuffer();
	}

	//image should be in mp_buffer now
	//printf("frame retrieved\n");
	//printf("Image Length: %d\n", buf.bytesused);

	return PrimitiveBuffer(mp_buffer,buf.bytesused);
}

void WebCamera::uninit_mmap()
{
	//in case that the memory hasn't been mapped yet; map it
	if(m_mapped == false)
	{
		return;
	}

	if (munmap(mp_buffer, m_buffer_length) == -1)
	{
		perror("munmap");
	}

	m_mapped = false;

}


int WebCamera::openDevice(const std::string& dev)
{
	m_fd = open(dev.c_str(), O_RDWR);

	if (m_fd == -1)
	{
		perror("Opening video device");
		return -1;
	}

	m_dev = dev;

	return 0;
}

int WebCamera::closeDevice()
{
	return close(m_fd);
}

int WebCamera::print_caps()
{
	struct v4l2_capability caps = {};

	if (xioctl(m_fd, VIDIOC_QUERYCAP, &caps) == -1)
	{
		perror("Querying Capabilities");
		return 1;
	}

	printf( "Driver Caps:\n Driver: \"%s\"\n Card: \"%s\"\n Bus: \"%s\"\n Version: %d.%d\n Capabilities: %08x\n",
			caps.driver, caps.card, caps.bus_info, (caps.version>>16)&&0xff, (caps.version>>24)&&0xff, caps.capabilities);

	struct v4l2_cropcap cropcap = {0};

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl(m_fd, VIDIOC_CROPCAP, &cropcap) == -1)
	{
		perror("Querying Cropping Capabilities");
		return 1;
	}

	printf( "Camera Cropping:\n Bounds: %dx%d+%d+%d\n Default: %dx%d+%d+%d\n Aspect: %d/%d\n",
		cropcap.bounds.width, cropcap.bounds.height, cropcap.bounds.left, cropcap.bounds.top,
		cropcap.defrect.width, cropcap.defrect.height, cropcap.defrect.left, cropcap.defrect.top,
		cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);

	struct v4l2_fmtdesc fmtdesc = {0};
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	char fourcc[5] = {0};
	char c, e;

	printf(" FMT : CE Desc\n--------------------\n");

	while (xioctl(m_fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
	{
		strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);

		c = fmtdesc.flags & 1? 'C' : ' ';
		e = fmtdesc.flags & 2? 'E' : ' ';
		printf(" %s: %c%c %s\n", fourcc, c, e, fmtdesc.description);

		fmtdesc.index++;
	}


	return 0;
}

int WebCamera::setImageFormat(unsigned int width, unsigned int height)
{
	char fourcc[5] = {0};

	m_width = width;
	m_height = height;

	DELETE_NULL_CHECKING(m_image);
	m_image = new Lazarus::Image<unsigned char>(m_width,m_height,Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_RGB);

	//set the image format for capture
	struct v4l2_format fmt = {0};

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;

	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P;
	//fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (xioctl(m_fd, VIDIOC_S_FMT, &fmt) == -1)
	{
		perror("Setting Pixel Format");
		return 1;
	}

	strncpy(fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);
	printf( "Selected Camera Mode:\n Width: %d\n Height: %d\n PixFmt: %s\n Field: %d\n",
			fmt.fmt.pix.width, fmt.fmt.pix.height, fourcc, fmt.fmt.pix.field);

	return 0;
}

} /* namespace SHVideoSources */
