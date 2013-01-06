#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <linux/input.h>
#ifdef MARTII
#include <poll.h>
#include <stdint.h>
#endif

#include "io.h"

#define RC_DEVICE	"/dev/input/nevis_ir"

extern int instance;
struct input_event ev;
static unsigned short rccode=-1;
static int rc;

int InitRC(void)
{
	rc = open(RC_DEVICE, O_RDONLY);
	if(rc == -1)
	{
		perror("msgbox <open remote control>");
		exit(1);
	}
	fcntl(rc, F_SETFL, O_NONBLOCK | O_SYNC);
	while(RCKeyPressed());
	return 1;
}

int CloseRC(void)
{
	while(RCKeyPressed());
	close(rc);
	return 1;
}

int RCKeyPressed(void)
{
#ifdef MARTII
	static int repeat_count = 0;
#endif
	if(read(rc, &ev, sizeof(ev)) == sizeof(ev))
	{
#ifdef MARTII
		if(ev.value == 1)
#else
		if(ev.value)
#endif
		{
#ifdef MARTII
			repeat_count = 0;
#endif
			rccode=ev.code;
			return 1;
		}
#ifdef MARTII
		if (ev.value == 2 && ++repeat_count > 2) {
			rccode=ev.code;
		return 1;
		}
		repeat_count = 0;
#endif
	}
	rccode = -1;
	return 0;
}


#ifdef MARTII
int GetRCCode(int timeout_in_ms)
{
	int rv = -1;

	if (timeout_in_ms) {
		struct pollfd pfd;
		struct timeval tv;
		uint64_t ms_now, ms_final;

		pfd.fd = rc;
		pfd.events = POLLIN;
		pfd.revents = 0;

		gettimeofday( &tv, NULL );
		ms_now = tv.tv_usec/1000 + tv.tv_sec * 1000;
		if (timeout_in_ms > 0)
			ms_final = ms_now + timeout_in_ms;
		else
			ms_final = UINT64_MAX;
		while (ms_final > ms_now) {
			switch(poll(&pfd, 1, timeout_in_ms)) {
				case -1:
					perror("GetRCCode: poll() failed");
				case 0:
					return -1;
				default:
					;
			}
			if(RCKeyPressed()) {
				rv = rccode;
				while(RCKeyPressed());
				return rv;
			}

			gettimeofday( &tv, NULL );
			ms_now = tv.tv_usec/1000 + tv.tv_sec * 1000;
			if (timeout_in_ms > 0)
				timeout_in_ms = (int)(ms_final - ms_now);
		}
	} else if(RCKeyPressed()) {
		rv = rccode;
		while(RCKeyPressed());
	}
	return rv;
}
#else
int GetRCCode(void)
{
	int rv;
	
	if(!RCKeyPressed() || (get_instance()>instance))
	{
		return -1;
	}
	rv=rccode;
	while(RCKeyPressed());
	
	return rv;
}
#endif


