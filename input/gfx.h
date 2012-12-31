#ifndef __GFX_H__

#define __GFX_H__

void RenderBox(int sx, int sy, int ex, int ey, int mode, int color);
void RenderCircle(int sx, int sy, char col);
#ifdef MARTII
void FillRect(int _sx, int _sy, int _dx, int _dy, uint32_t color);
#endif

#endif
