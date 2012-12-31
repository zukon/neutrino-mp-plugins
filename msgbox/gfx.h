#ifndef __GFX_H__

#define __GFX_H__

void RenderBox(int sx, int sy, int ex, int ey, int mode, int color);
#ifdef MARTII
# ifdef HAVE_SPARK_HARDWARE
void FillRect(int sx, int sy, int ex, int ey, uint32_t color);
# endif
#endif

#endif
