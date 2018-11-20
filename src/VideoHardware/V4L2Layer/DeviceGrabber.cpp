/*
 * DeviceGrabber.cpp
 *
 *  Created on: 04.08.2011
 *      Author: nao
 */

#include "DeviceGrabber.h"

namespace Lazarus
{

DeviceGrabber::DeviceGrabber(int width, int height, unsigned int buffer_count, long long max_frames)
{

	this->dev_name        = NULL;
	this->io              = IO_METHOD_MMAP;//IO_METHOD_MMAP; IO_METHOD_READ; IO_METHOD_USERPTR;
	this->fd              = -1;
	this->buffers         = NULL;
	this->n_buffers       = 0;

	this->width=width;
	this->height = height;
	m_max_frames = max_frames;
	m_buffer_count = buffer_count;
	mp_callback_object = NULL;

}

DeviceGrabber::~DeviceGrabber()
{
	uninit_device();
}

void DeviceGrabber::errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror (errno));

	exit(EXIT_FAILURE);
}

int DeviceGrabber::xioctl(int fd, int request, void *arg)
{
	int r;

	do
	{
		r = ioctl (fd, request, arg);
	}
	while(-1 == r && EINTR == errno);

	return r;
}

int DeviceGrabber::getWidth()
{
	return this->width;
}

int DeviceGrabber::getHeight()
{
	return this->height;
}

void DeviceGrabber::process_image(const void* p)
{
	Lazarus::PrimitiveBuffer* campixdata = (Lazarus::PrimitiveBuffer*) p;

	//hand the image buffer to the callback object
	mp_callback_object->setImageBuffer(campixdata);

	//execute the callback
	mp_callback_object->callbackMethod(this);

}

int DeviceGrabber::read_frame(void)
{
	struct v4l2_buffer buf;
	unsigned int i =0;

	switch (io)
	{
	case IO_METHOD_READ:
		if(-1 == read(fd, buffers[0]->mp_data_, buffers[0]->m_length_))
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("read");
			}
		}

		//process_image(buffers[0]->start);
		process_image(buffers[0]);

		break;

	case IO_METHOD_MMAP:
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

		assert(buf.index < n_buffers);

		//process_image(buffers[buf.index]->start);
		process_image(buffers[buf.index]);

		if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		{
				errno_exit ("VIDIOC_QBUF");
		}

		break;

	case IO_METHOD_USERPTR:
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if(-1 == xioctl (fd, VIDIOC_DQBUF, &buf))
		{
			switch(errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}

		for(i = 0; i < n_buffers; ++i)
		{
				if(buf.m.userptr == (unsigned long) buffers[i]->mp_data_
					&& buf.length == buffers[i]->m_length_)
				{
						break;
				}
		}

		assert(i < n_buffers);

		process_image((void *) buf.m.userptr);

		if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		{
			errno_exit("VIDIOC_QBUF");
		}

		break;
	}

	return 1;
}

void DeviceGrabber::setCallback(DeviceGrabberCallback* callback_object)
{
	mp_callback_object = callback_object;
}


void DeviceGrabber::mainloop(void)
{
	int frame_counter = 0;

	//continue until all desired frames have been read or the thread is stopped
	while(frame_counter < m_max_frames && m_thread_active == true)
	{
		while(1)
		{
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO (&fds);
			FD_SET (fd, &fds);

			/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select (fd + 1, &fds, NULL, NULL, &tv);

			if(r == -1)
			{
					if (EINTR == errno)
					{
						continue;
					}

					errno_exit("select");
			}

			if(r == 0)
			{
					fprintf (stderr, "select timeout\n");
					reset();
					return;
					//exit (EXIT_FAILURE);
			}

			if(read_frame())
					break;

			/* EAGAIN - continue select loop. */
		}
	}
}

void DeviceGrabber::stop_capturing(void)
{
	enum v4l2_buf_type type;

	switch (io)
	{
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:

	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
		{
				errno_exit ("VIDIOC_STREAMOFF");
		}

		break;
	}
}

void DeviceGrabber::start_capturing(void)
{
	enum v4l2_buf_type type;

	switch (io)
	{
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
		for (unsigned int i = 0; i < n_buffers; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;
			printf("asd\n");
			if (xioctl(fd, VIDIOC_QBUF, &buf) == -1)
			{
				errno_exit("VIDIOC_QBUF");
			}
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (fd, VIDIOC_STREAMON, &type) == -1)
		{
			errno_exit("VIDIOC_STREAMON");
		}

		break;

	case IO_METHOD_USERPTR:
		for(unsigned int i = 0; i < n_buffers; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_USERPTR;
			buf.index       = i;
			buf.m.userptr   = (unsigned long) buffers[i]->mp_data_;
			buf.length      = buffers[i]->m_length_;

			if (xioctl(fd, VIDIOC_QBUF, &buf) == -1)
			{
				errno_exit("VIDIOC_QBUF");
			}
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (fd, VIDIOC_STREAMON, &type) == -1)
		{
			errno_exit("VIDIOC_STREAMON");
		}

		break;
	}
}

void DeviceGrabber::uninit_device(void)
{
	switch (io)
	{
	case IO_METHOD_READ:
		free(buffers[0]->mp_data_);
		break;

	case IO_METHOD_MMAP:
		for (unsigned int i = 0; i < m_buffer_count; ++i)
		{
			if (munmap(buffers[i]->mp_data_, buffers[i]->m_length_) == -1)
			{
				errno_exit("munmap");
			}
		}
		break;

	case IO_METHOD_USERPTR:
		for (unsigned int i = 0; i < m_buffer_count; ++i)
		{
			free(buffers[i]->mp_data_);
		}

		break;
	}

	delete(buffers);
}

void DeviceGrabber::init_read(unsigned int buffer_size)
{
        //buffers-> = (buffer*)calloc (1, sizeof (*buffers));
		*(this->buffers) = new Lazarus::PrimitiveBuffer();

        if(!buffers)
        {
			fprintf (stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
        }

        buffers[0]->m_length_ = buffer_size;
        buffers[0]->mp_data_ = (unsigned char*)malloc(buffer_size);

        if (!buffers[0]->mp_data_)
        {
			fprintf (stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
        }
}

void DeviceGrabber::init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count               = m_buffer_count;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if(xioctl(fd, VIDIOC_REQBUFS, &req) == -1)
	{
		if(EINVAL == errno)
		{
			fprintf(stderr, "%s does not support memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		}
		else
		{
			errno_exit ("VIDIOC_REQBUFS");
		}
	}

	if(req.count < 2)
	{
		fprintf (stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}

	//buffers = (buffer*)calloc (req.count, sizeof (*buffers));
	this->buffers = new Lazarus::PrimitiveBuffer*[req.count];

	for(unsigned int i=0;i<req.count;i++)
	{
		*(this->buffers+i)=new Lazarus::PrimitiveBuffer();
	}

	if(!buffers)
	{
		fprintf (stderr, "Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for(n_buffers = 0; n_buffers < req.count; ++n_buffers)
	{
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
		{
			errno_exit ("VIDIOC_QUERYBUF");
		}

		buffers[n_buffers]->m_length_ = buf.length;
		buffers[n_buffers]->mp_data_ = (unsigned char*)
				 mmap(NULL /* start anywhere */,
					  buf.length,
					  PROT_READ | PROT_WRITE /* required */,
					  MAP_SHARED /* recommended */,
					  fd, buf.m.offset);

		if (buffers[n_buffers]->mp_data_ == MAP_FAILED)
		{
			errno_exit("mmap");
		}
	}
}

void DeviceGrabber::init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size;

	page_size = getpagesize();
	buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

	CLEAR (req);

	req.count = m_buffer_count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if(xioctl(fd, VIDIOC_REQBUFS, &req) == -1)
	{
		if(EINVAL == errno)
		{
			fprintf(stderr, "%s does not support user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		}
		else
		{
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	//buffers = (buffer*)calloc (4, sizeof (*buffers));
	for(unsigned int i=0;i<m_buffer_count;i++)
	{
		*(this->buffers+i)=new Lazarus::PrimitiveBuffer();
	}

	if(!buffers)
	{
		fprintf (stderr, "Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for(n_buffers = 0; n_buffers < m_buffer_count; ++n_buffers)
	{
		buffers[n_buffers]->m_length_ = buffer_size;

#ifdef LINUX
		buffers[n_buffers]->mp_data_ = (unsigned char*)memalign(/* boundary */ page_size, buffer_size);
#endif

#ifdef OSX
		posix_memalign((void**)&(buffers[n_buffers]->mp_data_),/* boundary */ page_size, buffer_size);
#endif

		if(!buffers[n_buffers]->mp_data_)
		{
			fprintf (stderr, "Out of memory\n");
			exit (EXIT_FAILURE);
		}
	}
}


void DeviceGrabber::init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if(-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap))
	{
		if(EINVAL == errno)
		{
			fprintf(stderr, "%s is no V4L2 device\n",
					 dev_name);
			exit(EXIT_FAILURE);
		}
		else
		{
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
			fprintf(stderr, "%s is no video capture device\n", dev_name);
			exit (EXIT_FAILURE);
	}

	switch (io)
	{
	case IO_METHOD_READ:
		if(!(cap.capabilities & V4L2_CAP_READWRITE))
		{
			fprintf(stderr, "%s does not support read i/o\n",
					 dev_name);
			exit(EXIT_FAILURE);
		}

		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if(!(cap.capabilities & V4L2_CAP_STREAMING))
		{
			fprintf(stderr, "%s does not support streaming i/o\n",
					 dev_name);
			exit(EXIT_FAILURE);
		}

		break;
	}


	/* Select video input, video standard and tune here. */
	CLEAR (cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if(0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
	{
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if(-1 == xioctl (fd, VIDIOC_S_CROP, &crop))
		{
			switch(errno)
			{
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
			}
		}
	}
	else
	{
			/* Errors ignored. */
	}


	CLEAR(fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = this->width;//640;
	fmt.fmt.pix.height      = this->height;//480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field       = V4L2_FIELD_ANY;//V4L2_FIELD_INTERLACED;


	if(-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
	{
		errno_exit ("VIDIOC_S_FMT");
	}

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
	{
			fmt.fmt.pix.bytesperline = min;
	}

	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;

	if (fmt.fmt.pix.sizeimage < min)
	{
			fmt.fmt.pix.sizeimage = min;
	}

	switch (io)
	{
	case IO_METHOD_READ:
		init_read(fmt.fmt.pix.sizeimage);
		break;

	case IO_METHOD_MMAP:
		init_mmap();
		break;

	case IO_METHOD_USERPTR:
		init_userp(fmt.fmt.pix.sizeimage);
		break;
	}
}

void DeviceGrabber::close_device(void)
{
	if(-1 == close (fd))
	{
			errno_exit ("close");
	}

	fd = -1;
}

void DeviceGrabber::open_device(void)
{
	struct stat st;

	if(-1 == stat (dev_name, &st))
	{
		printf("Cannot identify '%s': %d, %s\n", dev_name, errno, strerror (errno));
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	if(!S_ISCHR (st.st_mode))
	{
		printf("%s is no device\n", dev_name);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if(-1 == fd)
	{
		printf("Cannot open '%s': %d, %s\n", dev_name, errno, strerror (errno));
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
}


void DeviceGrabber::run()
{
	printf("grabber thread started\n");
	open_device();
	printf("device opened\n");
	init_device();
	printf("device initialized\n");
	start_capturing();
	printf("capture started\n");
	mainloop();
	printf("mainloop finished\n");
	stop_capturing();
	printf("capturing stopped\n");
	uninit_device();
	printf("device uninitialized\n");
	close_device();
	printf("device closed\n");

	return;
}

void DeviceGrabber::reset()
{
    printf("reset !\n");
    stop_capturing();
    uninit_device();
    close_device();

    open_device();
    init_device();
    start_capturing();
    mainloop();
}

}
