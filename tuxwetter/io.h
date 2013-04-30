#ifndef __IO_H__

#define __IO_H__

#define RC_DEVICE	"/dev/input/nevis_ir"
#ifdef MARTII
#define RC_DEVICE_FALLBACK "/dev/input/event0"
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
