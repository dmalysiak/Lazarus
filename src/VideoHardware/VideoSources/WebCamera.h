/*
 * Camera.h
 *
 *  Created on: Sep 27, 2013
 *      Author: clustro
 */

#ifndef WEBCAMERA_H_
#define WEBCAMERA_H_

#include "../../BaseObject.h"
#include "../../PrimitiveBuffer.h"
#include "../../DataStructures/Image.h"

#include <string>

namespace Lazarus {

class WebCamera: public Lazarus::BaseObject {
public:
	WebCamera();
	virtual ~WebCamera();

	inline bool isStreaming()
	{
		return m_isStreaming;
	}

	virtual unsigned char* getFrame();
	int print_caps();
	int openDevice(const std::string& dev);
	int closeDevice();
	int startStream();
	int stopStream();
	int setImageFormat(unsigned int width, unsigned int height);

	/*
	 * Do not delete the returned object.
	 */
	Lazarus::Image<unsigned char>* capture_image();

	/**
	 * Returns the raw camera data.
	 * */
	Lazarus::PrimitiveBuffer capture_image_buffer();

	static int xioctl(int fh, int request, void *arg);


private:
	bool m_isStreaming;
	unsigned int m_buffer_length;
	std::string m_dev;
	int m_fd;
	unsigned char* mp_buffer;
	bool m_mapped;
	unsigned int m_width;
	unsigned int m_height;

	Lazarus::Image<unsigned char>* m_image;

	int init_mmap();
	void uninit_mmap();
	void reset();
};

} /* namespace SHVideoSources */
#endif /* WEBCAMERA_H_ */
