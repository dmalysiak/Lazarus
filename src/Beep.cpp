/*
 * Beep.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: Darius Malysiak
 */

#include "Beep.h"

namespace Lazarus {

Beep::Beep() {

	m_default_freq = 440.0; /* Middle A */
	m_default_length  =   200;   /* milliseconds */
	m_default_reps    =    1;
	m_default_delay   =   100;   /* milliseconds */
	m_console_fd = -1;
	m_default_end_delay = 0;

	//signal(SIGINT, handle_signal);
}

Beep::~Beep() {

	if(m_console_fd >= 0)
	{
	  /* Kill the sound, quit gracefully */
	  ioctl(m_console_fd, KIOCSOUND, 0);
	  close(m_console_fd);
	}

}

void Beep::stopBeepSystemWide()
{
	if(m_console_fd >= 0)
	{
	  /* Kill the sound, quit gracefully */
	  ioctl(m_console_fd, KIOCSOUND, 0);
	}
	else
	{
		/* try to snag the console */
		if((m_console_fd = open("/dev/console", O_WRONLY)) == -1)
		{
			printf("Could not open /dev/console for writing.\n");
			printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
		}

		/* Kill the sound, quit gracefully */
		ioctl(m_console_fd, KIOCSOUND, 0);

		close(m_console_fd);
	}
}

void Beep::handle_signal(int signum)
{
	switch(signum)
	{
	  case SIGINT:
	    if(m_console_fd >= 0)
	    {
	      /* Kill the sound, quit gracefully */
	      ioctl(m_console_fd, KIOCSOUND, 0);
	      close(m_console_fd);
	      exit(signum);
	    }
	    else
	    {
	      /* Just quit gracefully */
	      exit(signum);
	    }
	}
}

void Beep::playBeep()
{
	beep_params params;

	params.delay = m_default_delay;
	params.end_delay = m_default_end_delay;
	params.freq = m_default_freq;
	params.length = m_default_length;
	params.reps = m_default_reps;

	playBeep(params);
}

void Beep::playBeep(beep_params params)
{

  /* try to snag the console */
  if((m_console_fd = open("/dev/console", O_WRONLY)) == -1) {
    fprintf(stderr, "Could not open /dev/console for writing.\n");
    printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
    perror("open");
    exit(1);
  }

  /* Beep */
  for(int i = 0; i < params.reps; i++)
  {                    /* start beep */
	if(ioctl(m_console_fd, KIOCSOUND, (int)((float)m_tick_rate/params.freq)) < 0)
	{
	  printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
	  perror("ioctl");
	}
	/* Look ma, I'm not ansi C compatible! */
	usleep(1000*params.length);                          /* wait...    */
	ioctl(m_console_fd, KIOCSOUND, 0);                    /* stop beep  */
	if(params.end_delay || (i+1 < params.reps))
	{
	   usleep(1000*params.delay);                        /* wait...    */
	}
  }                                                     /* repeat.    */

  close(m_console_fd);
}


} /* namespace Lazarus */
