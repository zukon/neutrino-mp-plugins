#ifndef __IO_H__

#define __IO_H__

#include <config.h>
#define RC_DEVICE	"/dev/input/nevis_ir"
#ifdef MARTII
#ifdef HAVE_DUCKBOX_HARDWARE
#define RC_DEVICE_FALLBACK "/dev/input/event0"
#else
#define RC_DEVICE_FALLBACK "/dev/input/event1"
#endif
#endif

int InitRC(void);
int CloseRC(void);
int RCKeyPressed(void);
#ifdef MARTII
int GetRCCode(int);
void ClearRC(void);
#else
int GetRCCode(void);
#endif

#endif
