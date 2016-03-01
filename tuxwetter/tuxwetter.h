#ifndef __TUXWETTER_H__

#define __TUXWETTER_H__

#include <config.h>
#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#if HAVE_DVB_API_VERSION == 3
#include <linux/input.h>
#endif
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifdef MARTII
#include <stdint.h>
#ifdef HAVE_SPARK_HARDWARE
#include <linux/stmfb.h>
#define DEFAULT_XRES 1280
#define DEFAULT_YRES 720
extern int sync_blitter;
#endif
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_CACHE_SMALL_BITMAPS_H

#define BUFSIZE 	4095

#define MISS_FILE	"/var/tuxbox/config/tuxwetter/missing_translations.txt"

enum {LEFT, CENTER, RIGHT};

FT_Error 		error;
FT_Library		library;
FTC_Manager		manager;
FTC_SBitCache		cache;
FTC_SBit		sbit;
#if FREETYPE_MAJOR == 2 && FREETYPE_MINOR == 0
FTC_Image_Desc		desc;
#else
FTC_ImageTypeRec	desc;
#endif
FT_Face			face;
FT_UInt			prev_glyphindex;
FT_Bool			use_kerning;

// rc codes

#undef KEY_EPG
#undef KEY_SAT
#undef KEY_STOP
#undef KEY_PLAY

#define KEY_1			 		2
#define KEY_2			 		3
#define KEY_3			 		4
#define KEY_4			 		5
#define KEY_5			 		6
#define KEY_6			 		7
#define KEY_7			 		8
#define KEY_8			 		9
#define KEY_9					10
#define KEY_BACKSPACE           14
#define KEY_UP                  103
#define KEY_LEFT                105
#define KEY_RIGHT               106
#define KEY_DOWN                108
#define KEY_MUTE                113
#define KEY_VOLUMEDOWN          114
#define KEY_VOLUMEUP            115
#define KEY_POWER               116
#define KEY_HELP                138
#define KEY_HOME                102
#define KEY_EXIT				174
#define KEY_SETUP               141
#define KEY_PAGEUP              104
#define KEY_PAGEDOWN            109
#define KEY_OK           		0x160
#define KEY_RED          		0x18e
#define KEY_GREEN        		0x18f
#define KEY_YELLOW       		0x190
#define KEY_BLUE         		0x191

#define KEY_TVR					0x179
#define KEY_TTX					0x184
#define KEY_COOL				0x1A1
#define KEY_FAV					0x16C
#define KEY_EPG					0x16D
#define KEY_VF					0x175

#define KEY_SAT					0x17D
#define KEY_SKIPP				0x197
#define KEY_SKIPM				0x19C
#define KEY_TS					0x167
#define KEY_AUDIO				0x188
#define KEY_REW					0x0A8
#define KEY_FWD					0x09F
#define KEY_HOLD				0x077
#define KEY_REC					0x0A7
#define KEY_STOP				0x080
#define KEY_PLAY				0x0CF

//devs

int fb;

//framebuffer stuff

enum {FILL, GRID};

enum {CMCST, CMCS, CMCT, CMC, CMCIT, CMCI, CMHT, CMH, WHITE, BLUE0, GTRANSP, CMS, ORANGE, GREEN, YELLOW, RED, CMCP0, CMCP1, CMCP2, CMCP3};
#define TRANSP 0

extern int FSIZE_BIG;
extern int FSIZE_MED;
extern int FSIZE_SMALL;
extern int FSIZE_VSMALL;
extern int TABULATOR;

#ifdef MARTII
extern uint32_t *lfb, *lbb;
extern char *proxyadress, *proxyuserpwd;
#else
extern unsigned char *lfb, *lbb;
extern unsigned char *proxyadress, *proxyuserpwd;
#endif
struct fb_fix_screeninfo fix_screeninfo;
struct fb_var_screeninfo var_screeninfo;
#ifdef MARTII
extern uint32_t bgra[];
extern int stride;
#else
extern unsigned char rd[],gn[],bl[],tr[];
#endif

int startx, starty, sx, ex, sy, ey, debounce, rblock;
#ifndef MARTII
extern unsigned sc[8], tc[8];
#endif
extern int instance;
int get_instance(void);
void put_instance(int pval);

#ifndef FB_DEVICE
#define FB_DEVICE	"/dev/fb/0"
#endif
#ifndef FB_DEVICE_FALLBACK
#define FB_DEVICE_FALLBACK	"/dev/fb0"
#endif

#endif

