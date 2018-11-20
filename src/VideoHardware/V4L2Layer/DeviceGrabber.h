/*
 * DeviceGrabber.h
 *
 *  Created on: 04.08.2011
 *      Author: nao
 */

#ifndef DEVICEGRABBER_H_
#define DEVICEGRABBER_H_

#include "../../Threading/Thread.h"
#include "../../PrimitiveBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include "Defines.h"
#include "DeviceGrabberCallback.h"

#include <malloc.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

namespace Lazarus
{

class DeviceGrabber : public Lazarus::Thread{
public:
	DeviceGrabber(int width, int height, unsigned int buffer_count = 4, long long max_frames = -1);
	virtual ~DeviceGrabber();

	void mainloop(void);

	void setCallback(DeviceGrabberCallback* callback_object);


	int getWidth();
	int getHeight();

	void run();


private:
	void process_image(const void *p);
	int read_frame(void);
	void reset();
	void close_device(void);
	void open_device(void);
	void stop_capturing(void);
	void start_capturing(void);
	void uninit_device(void);
	void init_read(unsigned int buffer_size);
	void init_mmap(void);
	void init_userp(unsigned int buffer_size);
	void init_device(void);
	void errno_exit(const char *s);
	int xioctl(int fd, int request, void *arg);
	int width;
	int height;
	long long m_max_frames;
	char*            dev_name;
	io_method        io;
	int              fd;
	Lazarus::PrimitiveBuffer**          buffers;
	unsigned int     n_buffers;//just a global counting var
	unsigned int m_buffer_count;

	DeviceGrabberCallback* mp_callback_object;
};

}

#endif /* DEVICEGRABBER_H_ */
