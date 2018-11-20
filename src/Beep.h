/*
 * Beep.h
 *
 *  Created on: Jun 29, 2014
 *      Author: Darius Malysiak
 */

#ifndef BEEP_H_
#define BEEP_H_

#include "BaseObject.h"

#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kd.h>

/*
 * This class is based on the work of Johnathan Nightingale
 */

namespace Lazarus {

struct beep_params {
  float freq;     /* tone frequency (Hz)      */
  int length;     /* tone length    (ms)      */
  int reps;       /* # of repetitions         */
  int delay;      /* delay between reps  (ms) */
  int end_delay;  /* do we delay after last rep? */
};

class Beep: public Lazarus::BaseObject {
public:
	Beep();
	virtual ~Beep();

	void playBeep(beep_params parms);

	/*
	 * plays a beep with the default values
	 */
	void playBeep();

	/*
	 * stops every active beep on the local system
	 */
	void stopBeepSystemWide();

private:

	/*  This number represents the fixed frequency of the original PC XT's
	   timer chip (the 8254 AFAIR), which is approximately 1.193 MHz. This
	   number is divided with the desired frequency to obtain a counter value,
	   that is subsequently fed into the timer chip, tied to the PC speaker.
	   The chip decreases this counter at every tick (1.193 MHz) and when it
	   reaches zero, it toggles the state of the speaker (on/off, or in/out),
	   resets the counter to the original value, and starts over. The end
	   result of this is a tone at approximately the desired frequency. :)
	*/

	const unsigned int m_tick_rate = 1193180;

	/* Meaningful Defaults */
	float m_default_freq ; /* Middle A */
	unsigned int m_default_length;   /* milliseconds */
	unsigned int m_default_reps;
	unsigned int m_default_delay;   /* milliseconds */
	int m_default_end_delay;


	int m_console_fd;

	void handle_signal(int signum);

};

} /* namespace Lazarus */

#endif /* BEEP_H_ */
