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
#include "tuxwetter.h"
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
	if(read(rc, &ev, sizeof(ev)) == sizeof(ev))
	{
#ifdef MARTII
		static int repeat_count = 0;
		switch(ev.value) {
		case 0:
			repeat_count = 0;
			break;
		case 1:
			repeat_count = 0;
			rccode=ev.code;
			return 1;
		case 2:
			if (++repeat_count > 1) {
				repeat_count = 0;
				rccode=ev.code;
				return 1;
			}
			break;
		}
#else
		if(ev.value)
		{
			rccode=ev.code;
			return 1;
		}
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


