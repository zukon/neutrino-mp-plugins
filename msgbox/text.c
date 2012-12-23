#include "text.h"
#include "gfx.h"
#include "io.h"
#ifdef MARTII
#include "msgbox.h"
#endif

int FSIZE_BIG=28;
int FSIZE_MED=24;
int FSIZE_SMALL=20;
int TABULATOR=72;

/******************************************************************************
 * MyFaceRequester
 ******************************************************************************/

FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface)
{
	FT_Error result;

	result = FT_New_Face(library, face_id, 0, aface);

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
	FT_Error error;

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
	};

	//load char

		if(!(glyphindex = FT_Get_Char_Index(face, currentchar)))
		{
			printf("TuxCom <FT_Get_Char_Index for Char \"%c\" failed\n", (int)currentchar);
			return 0;
		}


		if((error = FTC_SBitCache_Lookup(cache, &desc, glyphindex, &sbit, NULL)))
		{
			printf("TuxCom <FTC_SBitCache_Lookup for Char \"%c\" failed with Errorcode 0x%.2X>\n", (int)currentchar, error);
			return 0;
		}

		if(use_kerning)
		{
			FT_Get_Kerning(face, prev_glyphindex, glyphindex, ft_kerning_default, &kerning);

			prev_glyphindex = glyphindex;
			kerning.x >>= 6;
		}
		else
			kerning.x = 0;

		//render char

		if(color != -1) /* don't render char, return charwidth only */
		{
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
int GetStringLen(int sx, char *string, int size)
#else
int GetStringLen(int sx, unsigned char *string, int size)
#endif
{
int i, found;
int stringlen = 0;

	//reset kerning

		prev_glyphindex = 0;

	//calc len

		if(size)
		{
			desc.width = desc.height = size;
		}
		
		while(*string != '\0')
		{
			if(*string != '~')
			{
				stringlen += RenderChar(*string, -1, -1, -1, -1);
			}
			else
			{
				string++;
				if(*string=='t')
				{
					stringlen=desc.width+TABULATOR*((int)(stringlen/TABULATOR)+1);
				}
				else
				{
					if(*string=='T')
					{
						if(sscanf(string+1,"%4d",&i)==1)
						{
							string+=4;
							stringlen=i-sx;
						}
					}
					else
					{
						found=0;
#ifdef MARTII
						for(i=0; i<(int)sizeof(sc) && !found; i++)
#else
						for(i=0; i<sizeof(sc) && !found; i++)
#endif
						{
							if(*string==sc[i])
							{
								stringlen += RenderChar(tc[i], -1, -1, -1, -1);
								found=1;
							}
						}
					}
				}
			}
			string++;
		}

	return stringlen;
}


void CatchTabs(char *text)
{
	int i;
	char *tptr=text;
	
	while((tptr=strstr(tptr,"~T"))!=NULL)
	{
		*(++tptr)='t';
		for(i=0; i<4; i++)
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
	int stringlen, ex, charwidth,i,found;
	char rstr[BUFSIZE], *rptr=rstr, rc;
	int varcolor=color;

	//set size
	
		strcpy(rstr,string);

		desc.width = desc.height = size;
		TABULATOR=3*size;
	//set alignment

		stringlen = GetStringLen(sx, rstr, size);

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

#ifdef MARTII
		if(sync_blitter && ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
			perror("RenderString ioctl STMFBIO_SYNC_BLITTER");
#endif
		while(*rptr != '\0')
		{
			if(*rptr=='~')
			{
				++rptr;
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
							if(sscanf(rptr+1,"%4d",&i)==1)
							{
								rptr+=4;
								sx=i;
							}
						break;
					}
				}
			}
			else
			{
#ifdef MARTII
				if (*rptr==0xC3)
				{
					++rptr;
					switch(*rptr)
					{
					case 0x84: *rptr='Ä'; break;
					case 0x96: *rptr='Ö'; break;
					case 0x9C: *rptr='Ü'; break;
					case 0xA4: *rptr='ä'; break;
					case 0xB6: *rptr='ö'; break;
					case 0xBC: *rptr='ü'; break;
					case 0x9F: *rptr='ß'; break;
					default  : *rptr='.'; break;
					}
				}
#endif
				if((charwidth = RenderChar(*rptr, sx, sy, ex, varcolor)) == -1) return sx; /* string > maxwidth */
				sx += charwidth;
			}
			rptr++;
		}
	return stringlen;
}

