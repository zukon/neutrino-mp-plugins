#include "text.h"
#include "gfx.h"
#include "io.h"
#ifdef MARTII
#include "shellexec.h"
#endif

int FSIZE_BIG=28;
int FSIZE_MED=24;
int FSIZE_SMALL=20;
int TABULATOR=72;

#ifdef MARTII
static char unsigned sc[8]={'a','o','u','A','O','U','z','d'}, tc[8]={'ä','ö','ü','Ä','Ö','Ü','ß','°'}, su[7]={0xA4,0xB6,0xBC,0x84,0x96,0x9C,0x9F};
extern int sync_blitter;
extern void blit();
#else
static unsigned sc[8]={'a','o','u','A','O','U','z','d'}, tc[8]={'ä','ö','ü','Ä','Ö','Ü','ß','°'}, su[7]={0xA4,0xB6,0xBC,0x84,0x96,0x9C,0x9F};
#endif

void TranslateString(char *src)
{
#ifdef MARTII
unsigned
#endif
int i,found,quota=0;
char rc,*rptr=src,*tptr=src;

	while(*rptr != '\0')
	{
		if(*rptr=='\'')
		{
			quota^=1;
		}
		if(!quota && *rptr=='~')
		{
			++rptr;
			rc=*rptr;
			found=0;
			for(i=0; i<sizeof(sc) && !found; i++)
			for(i=0; i<sizeof(sc) && !found; i++)
			{
				if(rc==sc[i])
				{
					rc=tc[i];
					found=1;
				}
			}
			if(found)
			{
				*tptr=rc;
			}
			else
			{
				*tptr='~';
				tptr++;
				*tptr=*rptr;
			}
		}
		else
		{
			if (!quota && *rptr==0xC3 && *(rptr+1))
			{
				found=0;
				for(i=0; i<sizeof(su) && !found; i++)
				{
					if(*(rptr+1)==su[i])
					{
						found=1;
						*tptr=tc[i];
						++rptr;
					}
				}
				if(!found)
				{
					*tptr=*rptr;
				}
			}
			else
			{
				*tptr=*rptr;
			}
		}
		tptr++;
		rptr++;
	}
	*tptr=0;
}

/******************************************************************************
* MyFaceRequester
******************************************************************************/

FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface)
{
	FT_Error result;

	result = FT_New_Face(library, face_id, 0, aface);

	if(result) printf("shellexec <Font \"%s\" failed>\n", (char*)face_id);

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
#if defined(MARTII) && defined(HAVE_SPARK_HARDWARE)
			if(sync_blitter) {
				sync_blitter = 0;
				if (ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
					perror("RenderString ioctl STMFBIO_SYNC_BLITTER");
			}
#endif
			uint32_t bgcolor = *(lbb + (starty + _sy) * stride + (startx + _sx));
			//unsigned char pix[4]={bl[color],gn[color],rd[color],tr[color]};
			uint32_t fgcolor = bgra[color];
			uint32_t *colors = lookup_colors(fgcolor, bgcolor);
			uint32_t *p = lbb + (startx + _sx + sbit->left + kerning.x) + stride * (starty + _sy - sbit->top);
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
	
	switch (size)
	{
		case SMALL: desc.width = desc.height = FSIZE_SMALL; break;
		case MED:   desc.width = desc.height = FSIZE_MED; break;
		case BIG:   desc.width = desc.height = FSIZE_BIG; break;
		default:    desc.width = desc.height = size; break;
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


/******************************************************************************
 * RenderString
 ******************************************************************************/

void RenderString(char *string, int sx, int sy, int maxwidth, int layout, int size, int color)
{
	int stringlen, ex, charwidth, i;
	char rstr[256], *rptr=rstr;
	int varcolor=color;

	strcpy(rstr,string);
	if(strstr(rstr,"~c"))
		layout=CENTER;

	//set size

		switch (size)
		{
			case SMALL: desc.width = desc.height = FSIZE_SMALL; break;
			case MED:   desc.width = desc.height = FSIZE_MED; break;
			case BIG:   desc.width = desc.height = FSIZE_BIG; break;
			default:    desc.width = desc.height = size; break;
		}
		
	//set alignment

		if(layout != LEFT)
		{
			stringlen = GetStringLen(sx, string, size);

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
				switch(*rptr)
				{
					case 'R': varcolor=RED; break;
					case 'G': varcolor=GREEN; break;
					case 'Y': varcolor=YELLOW; break;
					case 'B': varcolor=BLUE0; break;
					case 'S': varcolor=color; break;
					case 't': sx=((sx/TABULATOR)+1)*TABULATOR; break;
					case 'T': 
						if(sscanf(rptr+1,"%4d",&i)==1)
						{
							rptr+=4;
							sx=i;
						}
						else
						{
							sx=((sx/TABULATOR)+1)*TABULATOR;
						}
				}
			}
			else
			{
				if((charwidth = RenderChar(*rptr, sx, sy, ex, ((color!=CMCIT) && (color!=CMCST))?varcolor:color)) == -1) return; /* string > maxwidth */
				sx += charwidth;
			}
			rptr++;
		}
}

/******************************************************************************
 * ShowMessage
 ******************************************************************************/

void remove_tabs(char *src)
{
int i;
char *rmptr, *rmstr, *rmdptr;

	if(src && *src)
	{
		rmstr=strdup(src);
		rmdptr=rmstr;
		rmptr=src;
		while(*rmptr)
		{
			if(*rmptr=='~')
			{
				++rmptr;
				if(*rmptr)
				{
					if(*rmptr=='t')
					{
						*(rmdptr++)=' ';
					}
					else
					{
						if(*rmptr=='T')
						{
							*(rmdptr++)=' ';
							i=4;
							while(i-- && *(rmptr++));
						}
					}
					++rmptr;
				}
			}
			else
			{
				*(rmdptr++)=*(rmptr++);
			}
		}
		*rmdptr=0;
		strcpy(src,rmstr);
		free(rmstr);
	}
}

void ShowMessage(char *mtitle, char *message, int wait)
{
	extern int radius;
	int ixw=400;
	int lx=startx/*, ly=starty*/;
	char *tdptr;
	
	startx = sx + (((ex-sx) - ixw)/2);
//	starty=sy;
	
	//layout

		RenderBox(0, 178, ixw, 327, radius, CMH);
		RenderBox(2, 180, ixw-4, 323, radius, CMC);
		RenderBox(0, 178, ixw, 220, radius, CMH);

	//message
		
		tdptr=strdup(mtitle);
		remove_tabs(tdptr);
		RenderString(tdptr, 2, 213, ixw, CENTER, FSIZE_BIG, CMHT);
		free(tdptr);
		tdptr=strdup(message);
		remove_tabs(tdptr);
		RenderString(tdptr, 2, 270, ixw, CENTER, FSIZE_MED, CMCT);
		free(tdptr);

		if(wait)
		{
			RenderBox(ixw/2-25, 286, ixw/2+25, 310, radius, CMCS);
			RenderString("OK", ixw/2-25, 305, 50, CENTER, FSIZE_MED, CMCT);
		}
#ifdef MARTII
		blit();
#else
		memcpy(lfb, lbb, fix_screeninfo.line_length*var_screeninfo.yres);
#endif

		while(wait && (GetRCCode() != RC_OK));
		
		startx=lx;
//		starty=ly;

}

