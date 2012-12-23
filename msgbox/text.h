#ifndef __TEXT_H__

#define __TEXT_H__

#include "msgbox.h"

FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface);
int RenderString(char *string, int sx, int sy, int maxwidth, int layout, int size, int color);
#ifdef MARTII
int GetStringLen(int sx, char *string, int size);
#else
int GetStringLen(int sx, unsigned char *string, int size);
#endif
void CatchTabs(char *text);

#endif
