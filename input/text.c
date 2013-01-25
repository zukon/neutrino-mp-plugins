#include "text.h"
#include "gfx.h"
#include "io.h"

int FSIZE_BIG=32;
int FSIZE_MED=24;
int FSIZE_SMALL=20;
int TABULATOR=300;

#ifdef MARTII
void blit(void);
extern int sync_blitter;
static char *sc = "aouAOUzd", *su= "\xA4\xB6\xBC\x84\x96\x9C\x9F", *tc="\xE4\xF6\xFC\xDE\xC4\xD6\xDC\xB0";
// from neutrino/src/driver/fontrenderer.cpp
int UTF8ToUnicode(char **textp, const int utf8_encoded) // returns -1 on error
{
	int unicode_value, i;
	char *text = *textp;
	if (utf8_encoded && ((((unsigned char)(*text)) & 0x80) != 0))
	{
		int remaining_unicode_length;
		if ((((unsigned char)(*text)) & 0xf8) == 0xf0) {
			unicode_value = ((unsigned char)(*text)) & 0x07;
			remaining_unicode_length = 3;
		} else if ((((unsigned char)(*text)) & 0xf0) == 0xe0) {
			unicode_value = ((unsigned char)(*text)) & 0x0f;
			remaining_unicode_length = 2;
		} else if ((((unsigned char)(*text)) & 0xe0) == 0xc0) {
			unicode_value = ((unsigned char)(*text)) & 0x1f;
			remaining_unicode_length = 1;
		} else {
			(*textp)++;
			return -1;
		}

		*textp += remaining_unicode_length;

		for (i = 0; *text && i < remaining_unicode_length; i++) {
			text++;
			if (((*text) & 0xc0) != 0x80) {
				remaining_unicode_length = -1;
				return -1;          // incomplete or corrupted character
			}
			unicode_value <<= 6;
			unicode_value |= ((unsigned char)(*text)) & 0x3f;
		}
	} else
		unicode_value = (unsigned char)(*text);

	(*textp)++;
	return unicode_value;
}

void CopyUTF8Char(char **to, char **from)
{
	int remaining_unicode_length;
	if (!((unsigned char)(**from) & 0x80))
		remaining_unicode_length = 1;
	else if ((((unsigned char)(**from)) & 0xf8) == 0xf0)
		remaining_unicode_length = 4;
	else if ((((unsigned char)(**from)) & 0xf0) == 0xe0)
		remaining_unicode_length = 3;
	else if ((((unsigned char)(**from)) & 0xe0) == 0xc0)
		remaining_unicode_length = 2;
	else {
		(*from)++;
		return;
	}
	while (**from && remaining_unicode_length) {
		**to = **from;
		(*from)++, (*to)++, remaining_unicode_length--;
	}
}

int isValidUTF8(char *text) {
	while (*text)
		if (-1 == UTF8ToUnicode(&text, 1))
			return 0;
	return 1;
}

void TranslateString(char *src, size_t size)
{
	char *fptr = src;
	size_t src_len = strlen(src);
	char *tptr_start = alloca(src_len * 4 + 1);
	char *tptr = tptr_start;

	if (isValidUTF8(src))
		strncpy(tptr_start, fptr, src_len + 1);
	else {
		while (*fptr) {
			int i;
			for (i = 0; tc[i] && (tc[i] != *fptr); i++);
			if (tc[i]) {
				*tptr++ = 0xC3;
				*tptr++ = su[i];
				fptr++;
			} else if (*fptr & 0x80)
				*fptr++;
			else
				*tptr++ = *fptr++;
		}
		*tptr = 0;
	}

	fptr = tptr_start;
	tptr = src;
	char *tptr_end = src + size - 5;
	while (*fptr && tptr < tptr_end) {
		if (*fptr == '~') {
			fptr++;
			int i;
			for (i = 0; sc[i] && (sc[i] != *fptr); i++);
			if (sc[i]) {
				*tptr++ = 0xC3;
				*tptr++ = su[i];
				fptr++;
			} else if (*fptr == 'd') {
				*tptr++ = 0xC2;
				*tptr++ = 0xb0;
				fptr++;
			} else
				*tptr++ = '~';
		} else
			CopyUTF8Char(&tptr, &fptr);
	}
	*tptr = 0;
}
#endif
/******************************************************************************
 * MyFaceRequester
 ******************************************************************************/

#ifdef MARTII
FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library lib, FT_Pointer request_data __attribute__((unused)), FT_Face *aface)
#else
FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface)
#endif
{
	FT_Error result;

#ifdef MARTII
	result = FT_New_Face(lib, face_id, 0, aface);
#else
	result = FT_New_Face(library, face_id, 0, aface);
#endif

	if(result) printf("msgbox <Font \"%s\" failed>\n", (char*)face_id);

	return result;
}

/******************************************************************************
 * RenderChar
 ******************************************************************************/

#ifdef MARTII
struct colors_struct
{
	uint32_t fgcolor, bgcolor;
	uint32_t colors[256];
};

#define COLORS_LRU_SIZE 16
static struct colors_struct *colors_lru_array[COLORS_LRU_SIZE] = { NULL };

static uint32_t *lookup_colors(uint32_t fgcolor, uint32_t bgcolor)
{
	struct colors_struct *cs;
	int i = 0, j;
	for (i = 0; i < COLORS_LRU_SIZE; i++)
		if (colors_lru_array[i] && colors_lru_array[i]->fgcolor == fgcolor && colors_lru_array[i]->bgcolor == bgcolor) {
			cs = colors_lru_array[i];
			for (j = i; j > 0; j--)
				colors_lru_array[j] = colors_lru_array[j - 1];
			colors_lru_array[0] = cs;
			return cs->colors;
		}
	i--;
	cs = colors_lru_array[i];
	if (!cs)
		cs = (struct colors_struct *) calloc(1, sizeof(struct colors_struct));
	for (j = i; j > 0; j--)
		colors_lru_array[j] = colors_lru_array[j - 1];
	cs->fgcolor = fgcolor;
	cs->bgcolor = bgcolor;

	int ro = var_screeninfo.red.offset;
	int go = var_screeninfo.green.offset;
	int bo = var_screeninfo.blue.offset;
	int to = var_screeninfo.transp.offset;
	int rm = (1 << var_screeninfo.red.length) - 1;
	int gm = (1 << var_screeninfo.green.length) - 1;
	int bm = (1 << var_screeninfo.blue.length) - 1;
	int tm = (1 << var_screeninfo.transp.length) - 1;
	int fgr = ((int)fgcolor >> ro) & rm;
	int fgg = ((int)fgcolor >> go) & gm;
	int fgb = ((int)fgcolor >> bo) & bm;
	int fgt = ((int)fgcolor >> to) & tm;
	int deltar = (((int)bgcolor >> ro) & rm) - fgr;
	int deltag = (((int)bgcolor >> go) & gm) - fgg;
	int deltab = (((int)bgcolor >> bo) & bm) - fgb;
	int deltat = (((int)bgcolor >> to) & tm) - fgt;
	for (i = 0; i < 256; i++)
		cs->colors[255 - i] =
			(((fgr + deltar * i / 255) & rm) << ro) |
			(((fgg + deltag * i / 255) & gm) << go) |
			(((fgb + deltab * i / 255) & bm) << bo) |
			(((fgt + deltat * i / 255) & tm) << to);

	colors_lru_array[0] = cs;
	return cs->colors;
}

int RenderChar(FT_ULong currentchar, int _sx, int _sy, int _ex, int color)
{
	int row, pitch;
	FT_UInt glyphindex;
	FT_Vector kerning;
	FT_Error err;

	if (currentchar == '\r') // display \r in windows edited files
	{
		if(color != -1)
		{
			if (_sx + 10 < _ex)
				RenderBox(_sx, _sy - 10, _sx + 10, _sy, GRID, color);
			else
				return -1;
		}
		return 10;
	}

	if (currentchar == '\t')
	{
		/* simulate horizontal TAB */
		return 15;
	}

	//load char

		if(!(glyphindex = FT_Get_Char_Index(face, currentchar)))
		{
			printf("TuxCom <FT_Get_Char_Index for Char \"%c\" failed\n", (int)currentchar);
			return 0;
		}

		if((err = FTC_SBitCache_Lookup(cache, &desc, glyphindex, &sbit, NULL)))
		{
			printf("TuxCom <FTC_SBitCache_Lookup for Char \"%c\" failed with Errorcode 0x%.2X>\n", (int)currentchar, error);
			return 0;
		}

		if(use_kerning)
		{
			FT_Get_Kerning(face, prev_glyphindex, glyphindex, ft_kerning_default, &kerning);

			prev_glyphindex = glyphindex;
			kerning.x >>= 6;
		} else
			kerning.x = 0;

		//render char

		if(color != -1) /* don't render char, return charwidth only */
		{
#if defined(HAVE_SPARK_HARDWARE)
			if(sync_blitter) {
				sync_blitter = 0;
				if (ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
					perror("RenderString ioctl STMFBIO_SYNC_BLITTER");
			}
#endif
			uint32_t bgcolor = *(lbb + (sy + _sy) * stride + (sx + _sx));
			//unsigned char pix[4]={bl[color],gn[color],rd[color],tr[color]};
			uint32_t fgcolor = bgra[color];
			uint32_t *colors = lookup_colors(fgcolor, bgcolor);
			uint32_t *p = lbb + (sx + _sx + sbit->left + kerning.x) + stride * (sy + _sy - sbit->top);
			uint32_t *r = p + (_ex - _sx);	/* end of usable box */
			for(row = 0; row < sbit->height; row++)
			{
				uint32_t *q = p;
				uint8_t *s = sbit->buffer + row * sbit->pitch;
				for(pitch = 0; pitch < sbit->width; pitch++)
				{
					if (*s)
							*q = colors[*s];
					q++, s++;
					if (q > r)	/* we are past _ex */
						break;
				}
				p += stride;
				r += stride;
			}
			if (_sx + sbit->xadvance >= _ex)
				return -1; /* limit to maxwidth */
		}

	//return charwidth

		return sbit->xadvance + kerning.x;
}
#else
int RenderChar(FT_ULong currentchar, int sx, int sy, int ex, int color)
{
//	unsigned char pix[4]={oldcmap.red[col],oldcmap.green[col],oldcmap.blue[col],oldcmap.transp[col]};
//	unsigned char pix[4]={0x80,0x80,0x80,0x80};
	unsigned char pix[4]={bl[color],gn[color],rd[color],tr[color]};
	int row, pitch, bit, x = 0, y = 0;
	FT_UInt glyphindex;
	FT_Vector kerning;
	FT_Error error;

	currentchar=currentchar & 0xFF;

	//load char

		if(!(glyphindex = FT_Get_Char_Index(face, (int)currentchar)))
		{
//			printf("msgbox <FT_Get_Char_Index for Char \"%c\" failed\n", (int)currentchar);
			return 0;
		}


		if((error = FTC_SBitCache_Lookup(cache, &desc, glyphindex, &sbit, NULL)))
		{
//			printf("msgbox <FTC_SBitCache_Lookup for Char \"%c\" failed with Errorcode 0x%.2X>\n", (int)currentchar, error);
			return 0;
		}

// no kerning used
/*
		if(use_kerning)
		{
			FT_Get_Kerning(face, prev_glyphindex, glyphindex, ft_kerning_default, &kerning);

			prev_glyphindex = glyphindex;
			kerning.x >>= 6;
		}
		else
*/
			kerning.x = 0;

	//render char

		if(color != -1) /* don't render char, return charwidth only */
		{
			if(sx + sbit->xadvance >= ex) return -1; /* limit to maxwidth */

			for(row = 0; row < sbit->height; row++)
			{
				for(pitch = 0; pitch < sbit->pitch; pitch++)
				{
					for(bit = 7; bit >= 0; bit--)
					{
						if(pitch*8 + 7-bit >= sbit->width) break; /* render needed bits only */

						if((sbit->buffer[row * sbit->pitch + pitch]) & 1<<bit) memcpy(lbb + (startx + sx + sbit->left + kerning.x + x)*4 + fix_screeninfo.line_length*(starty + sy - sbit->top + y),pix,4);

						x++;
					}
				}

				x = 0;
				y++;
			}

		}

	//return charwidth

		return sbit->xadvance + kerning.x;
}
#endif

/******************************************************************************
 * GetStringLen
 ******************************************************************************/

#ifdef MARTII
int GetStringLen(char *string, size_t size)
{
	int stringlen = 0;
	
	//reset kerning
	
	prev_glyphindex = 0;
	
	//calc len
	
	switch (size)
	{
		case SMALL: desc.width = desc.height = FSIZE_SMALL; break;
		case MED:   desc.width = desc.height = FSIZE_MED; break;
		case BIG:   desc.width = desc.height = FSIZE_BIG; break;
		default:    desc.width = desc.height = size; break;
	}

	while(*string)
		stringlen += RenderChar(UTF8ToUnicode(&string, 1), -1, -1, -1, -1);
	
	return stringlen;
}
#else
int GetStringLen(unsigned char *string, int size)
{
int i, found;
int stringlen = 0;

	//reset kerning

		prev_glyphindex = 0;

	//calc len

		switch (size)
		{
			case SMALL: desc.width = desc.height = FSIZE_SMALL; break;
			case MED:   desc.width = desc.height = FSIZE_MED; break;
			case BIG:   desc.width = desc.height = FSIZE_BIG; break;
			default:    desc.width = desc.height = size; break;
		}
		
		while(*string != '\0')
		{
			stringlen += RenderChar(*string, -1, -1, -1, -1);
			string++;
		}

	return stringlen;
}
#endif


void CatchTabs(char *text)
{
	int i;
	char *tptr=text;
	
	while((tptr=strstr(tptr,"~T"))!=NULL)
	{
		*(++tptr)='t';
		for(i=0; i<3; i++)
		{
			if(*(++tptr))
			{
				*tptr=' ';
			}
		}
	}
}

/******************************************************************************
 * RenderString
 ******************************************************************************/

int RenderString(char *string, int sx, int sy, int maxwidth, int layout, int size, int color)
{
#ifdef MARTII
	int stringlen, ex, charwidth,i;
	char rstr[BUFSIZE], *rptr=rstr;
#else
	int stringlen, ex, charwidth,i,found;
	char rstr[BUFSIZE], *rptr=rstr, rc;
#endif
	int varcolor=color;

	//set size
	
		strcpy(rstr,string);

		switch (size)
		{
			case SMALL: desc.width = desc.height = FSIZE_SMALL; break;
			case MED:   desc.width = desc.height = FSIZE_MED; break;
			case BIG:   desc.width = desc.height = FSIZE_BIG; break;
			default:    desc.width = desc.height = size; break;
		}
		TABULATOR=3*size;
	//set alignment

		stringlen = GetStringLen(rstr, size);

		if(layout != LEFT)
		{
			switch(layout)
			{
				case CENTER:	if(stringlen < maxwidth) sx += (maxwidth - stringlen)/2;
						break;

				case RIGHT:	if(stringlen < maxwidth) sx += maxwidth - stringlen;
			}
		}

	//reset kerning

		prev_glyphindex = 0;

	//render string

		ex = sx + maxwidth;

#if defined(MARTII) && defined(HAVE_SPARK_HARDWARE)
		if(sync_blitter) {
			sync_blitter = 0;
			if (ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
				perror("RenderString ioctl STMFBIO_SYNC_BLITTER");
		}
#endif
		while(*rptr != '\0')
		{
			if(*rptr=='~')
			{
				++rptr;
#ifndef MARTII
				rc=*rptr;
				found=0;
#ifdef MARTII
				for(i=0; i<(int)sizeof(sc) && !found; i++)
#else
				for(i=0; i<sizeof(sc) && !found; i++)
#endif
				{
					if(rc==sc[i])
					{
						rc=tc[i];
						found=1;
					}
				}
				if(found)
				{
					if((charwidth = RenderChar(rc, sx, sy, ex, varcolor)) == -1) return sx; /* string > maxwidth */
					sx += charwidth;
				}
				else
				{
#endif
					switch(*rptr)
					{
						case 'R': varcolor=RED; break;
						case 'G': varcolor=GREEN; break;
						case 'Y': varcolor=YELLOW; break;
						case 'B': varcolor=BLUE1; break;
						case 'S': varcolor=color; break;
						case 't':				
							sx=TABULATOR*((int)(sx/TABULATOR)+1);
							break;
						case 'T':
							if(sscanf(rptr+1,"%3d",&i)==1)
							{
								rptr+=3;
								sx=i;
							}
						break;
					}
#ifndef MARTII
				}
#endif
			}
			else
			{
#ifdef MARTII
				if((charwidth = RenderChar(UTF8ToUnicode(&rptr, 1), sx, sy, ex, varcolor)) == -1) return sx; /* string > maxwidth */
#else
				if((charwidth = RenderChar(*rptr, sx, sy, ex, varcolor)) == -1) return sx; /* string > maxwidth */
#endif
				sx += charwidth;
			}
#ifndef MARTII
			rptr++;
#endif
		}
	return stringlen;
}

