/*
 * $Id$
 *
 * clock - d-box2 linux project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
*/

#include <string.h>
#include <stdio.h>
#include <time.h>
#include "clock.h"
#include "text.h"
#include "gfx.h"

extern int FSIZE_BIG;
extern int FSIZE_MED;
extern int FSIZE_SMALL;

unsigned char FONT[64]= "/share/fonts/pakenham.ttf";
#define NCF_FILE "/var/tuxbox/config/neutrino.conf"
#define CFG_FILE "/var/tuxbox/config/clock.conf"
#define MAIL_FILE "/tmp/tuxmail.new"
//#define CFG_FILE "/tmp/clock.conf"


#define CL_VERSION  "0.14"

static char menucoltxt[CMH][24]={"Content_Selected_Text","Content_Selected","Content_Text","Content","Content_inactive_Text","inactive","Head_Text","Head"};

//					   CMCST,   CMCS,    CMCT,    CMC,     CMCIT,   CMCI,    CMHT,    CMH	
//					   WHITE,   BLUE0,   TRANSP,  FLASH,   ORANGE,  GREEN,   YELLOW,  RED
unsigned short rd[] = {0x00<<8, 0x32<<8, 0xc8<<8, 0x00<<8, 0x6e<<8, 0x00<<8, 0xbe<<8, 0x00<<8,
					   0xFF<<8, 0x00<<8, 0x00<<8, 0xFF<<8, 0xFF<<8, 0x00<<8, 0x7F<<8, 0x7F<<8};
unsigned short gn[] = {0x00<<8, 0x6e<<8, 0xc8<<8, 0x1e<<8, 0x8c<<8, 0x00<<8, 0x8c<<8, 0x14<<8,
					   0xFF<<8, 0x80<<8, 0x00<<8, 0xFF<<8, 0xC0<<8, 0x7F<<8, 0x7F<<8, 0x00<<8};
unsigned short bl[] = {0x00<<8, 0xc8<<8, 0xc8<<8, 0x46<<8, 0xaa<<8, 0x80<<8, 0x00<<8, 0x32<<8, 
					   0xFF<<8, 0xFF<<8, 0x80<<8, 0xFF<<8, 0x00<<8, 0x00<<8, 0x00<<8, 0x00<<8};
unsigned short tr[] = {0x00<<8, 0x14<<8, 0x00<<8, 0x14<<8, 0x00<<8, 0x0a<<8, 0x00<<8, 0x00<<8,
					   0x0000,  0x0A00,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000,  0x0000 };
					   
unsigned short ord[256], ogn[256], obl[256], otr[256];

struct fb_cmap colormap = {0, 256, ord, ogn, obl, otr};

unsigned char *lfb = 0, *lbb = 0;
char tstr[512];
int xpos = 540, ypos = 0, show_date = 0, big = 0, show_sec = 1, blink = 1, fcol = COL_WHITE, bcol = COL_BLACK, mail = 0;

int ExistFile(char *fname)
{
FILE *efh;

	if((efh=fopen(fname,"r"))==NULL)
	{
		return 0;
	}
	fclose(efh);
	return 1;
}

void TrimString(char *strg)
{
char *pt1=strg, *pt2=strg;

	while(*pt2 && *pt2<=' ')
	{
		++pt2;
	}
	if(pt1 != pt2)
	{
		do
		{
			*pt1=*pt2;
			++pt1;
			++pt2;
		}
		while(*pt2);
		*pt1=0;
	}
	while(strlen(strg) && strg[strlen(strg)-1]<=' ')
	{
		strg[strlen(strg)-1]=0;
	}
}

int ReadConf()
{
	FILE *fd_conf;
	char *cptr;

	//open config

	if(!(fd_conf = fopen(CFG_FILE, "r")))
	{
		return 0;
	}
	while(fgets(tstr, 511, fd_conf))
	{
		TrimString(tstr);
		if((tstr[0]) && (tstr[0]!='#') && (!isspace(tstr[0])) && ((cptr=strchr(tstr,'='))!=NULL))
		{
			if(strstr(tstr,"X") == tstr)
			{
				sscanf(cptr+1,"%d",&xpos);
			}
			if(strstr(tstr,"Y=") == tstr)
			{
				sscanf(cptr+1,"%d",&ypos);
			}
			if(strstr(tstr,"DATE=") == tstr)
			{
				sscanf(cptr+1,"%d",&show_date);
			}
			if(strstr(tstr,"BIG=") == tstr)
			{
				sscanf(cptr+1,"%d",&big);
			}
			if(strstr(tstr,"SEC=") == tstr)
			{
				sscanf(cptr+1,"%d",&show_sec);
			}
			if(strstr(tstr,"BLINK=") == tstr)
			{
				sscanf(cptr+1,"%d",&blink);
			}
			if(strstr(tstr,"FCOL=") == tstr)
			{
				sscanf(cptr+1,"%d",&fcol);
			}
			if(strstr(tstr,"BCOL=") == tstr)
			{
				sscanf(cptr+1,"%d",&bcol);
			}
			if(strstr(tstr,"MAIL=") == tstr)
			{
				sscanf(cptr+1,"%d",&mail);
			}
		}
	}
	return 1;
}

int Read_Neutrino_Cfg(char *entry)
{
FILE *nfh;
char tstr [512], *cfptr=NULL;
int rv=-1;

	if((nfh=fopen(NCF_FILE,"r"))!=NULL)
	{
		tstr[0]=0;

		while((!feof(nfh)) && ((strstr(tstr,entry)==NULL) || ((cfptr=strchr(tstr,'='))==NULL)))
		{
			fgets(tstr,500,nfh);
		}
		if(!feof(nfh) && cfptr)
		{
			++cfptr;
			if(sscanf(cfptr,"%d",&rv)!=1)
			{
				rv=-1;
			}
		//	printf("%s\n%s=%s -> %d\n",tstr,entry,cfptr,rv);
		}
		fclose(nfh);
	}
	return rv;
}

/******************************************************************************
 * Clock Main
 ******************************************************************************/

int main (int argc, char **argv)
{
	int tv,index,i,j,w,cmct=CMCT,cmc=CMC,trnspi=TRANSP,trnsp=0,found,loop=1,ms,mw;
	unsigned short int digits, digit_width, margin_left, margin_top_t, font_size, x4, margin_top_d, secs_width, newmail=0, mailgfx=0;
	time_t atim;
	struct tm *ltim;
	char *aptr,*rptr;
	char dstr[2]={0,0};
	FILE *tfh;

		printf("Clock Version %s\n",CL_VERSION);
		
		ReadConf();
	
		for(i=1; i<argc; i++)
		{
			aptr=argv[i];
			if((rptr=strchr(aptr,'='))!=NULL)
			{
				rptr++;
				if(strstr(aptr,"X=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						xpos=j;
					}
				}
				if(strstr(aptr,"Y=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						ypos=j;
					}
				}
				if(strstr(aptr,"DATE=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						show_date=j;
					}
				}
				if(strstr(aptr,"BIG=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						big=j;
					}
				}
				if(strstr(aptr,"SEC=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						show_sec=j;
					}
				}
				if(strstr(aptr,"BLINK=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						blink=j;
					}
				}
				if(strstr(aptr,"FCOL=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						fcol=j;
					}
				}
				if(strstr(aptr,"BCOL=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						bcol=j;
					}
				}
				if(strstr(aptr,"MAIL=")!=NULL)
				{
					if(sscanf(rptr,"%d",&j)==1)
					{
						mail=j;
					}
				}
			}
		}
		if((sx=Read_Neutrino_Cfg("screen_StartX"))<0)
			sx=80;
		
		if((ex=Read_Neutrino_Cfg("screen_EndX"))<0)
			ex=620;

		if((sy=Read_Neutrino_Cfg("screen_StartY"))<0)
			sy=80;

		if((ey=Read_Neutrino_Cfg("screen_EndY"))<0)
			ey=505;
			
		for(index=CMCST; index<=CMH; index++)
		{
			sprintf(tstr,"menu_%s_alpha",menucoltxt[index-1]);
			if((tv=Read_Neutrino_Cfg(tstr))>=0)
				tr[index-1]=(tv<<8);

			sprintf(tstr,"menu_%s_blue",menucoltxt[index-1]);
			if((tv=Read_Neutrino_Cfg(tstr))>=0)
				bl[index-1]=(tv+(tv<<8));

			sprintf(tstr,"menu_%s_green",menucoltxt[index-1]);
			if((tv=Read_Neutrino_Cfg(tstr))>=0)
				gn[index-1]=(tv+(tv<<8));

			sprintf(tstr,"menu_%s_red",menucoltxt[index-1]);
			if((tv=Read_Neutrino_Cfg(tstr))>=0)
				rd[index-1]=(tv+(tv<<8));
		}
		
		fb = open(FB_DEVICE, O_RDWR);

		if(ioctl(fb, FBIOGET_FSCREENINFO, &fix_screeninfo) == -1)
		{
			printf("Clock <FBIOGET_FSCREENINFO failed>\n");
			return -1;
		}
		if(ioctl(fb, FBIOGET_VSCREENINFO, &var_screeninfo) == -1)
		{
			printf("Clock <FBIOGET_VSCREENINFO failed>\n");
			return -1;
		}
		
		if(ioctl(fb, FBIOGETCMAP, &colormap) == -1)
		{
			printf("Clock <FBIOGETCMAP failed>\n");
			return -1;
		}

		if(!(lfb = (unsigned char*)mmap(0, fix_screeninfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0)))
		{
			printf("Clock <mapping of Framebuffer failed>\n");
			return -1;
		}

	//init fontlibrary

		if((error = FT_Init_FreeType(&library)))
		{
			printf("Clock <FT_Init_FreeType failed with Errorcode 0x%.2X>", error);
			munmap(lfb, fix_screeninfo.smem_len);
			return -1;
		}

		if((error = FTC_Manager_New(library, 1, 2, 0, &MyFaceRequester, NULL, &manager)))
		{
			printf("Clock <FTC_Manager_New failed with Errorcode 0x%.2X>\n", error);
			FT_Done_FreeType(library);
			munmap(lfb, fix_screeninfo.smem_len);
			return -1;
		}

		if((error = FTC_SBitCache_New(manager, &cache)))
		{
			printf("Clock <FTC_SBitCache_New failed with Errorcode 0x%.2X>\n", error);
			FTC_Manager_Done(manager);
			FT_Done_FreeType(library);
			munmap(lfb, fix_screeninfo.smem_len);
			return -1;
		}

		if((error = FTC_Manager_Lookup_Face(manager, FONT, &face)))
		{
			printf("Clock <FTC_Manager_Lookup_Face failed with Errorcode 0x%.2X>\n", error);
			FTC_Manager_Done(manager);
			FT_Done_FreeType(library);
			munmap(lfb, fix_screeninfo.smem_len);
			return -1;
		}

		use_kerning = FT_HAS_KERNING(face);

#ifdef FT_NEW_CACHE_API
		desc.face_id = FONT;
#else
		desc.font.face_id = FONT;
#endif
#if FREETYPE_MAJOR  == 2 && FREETYPE_MINOR == 0
		desc.image_type = ftc_image_mono;
#else
		desc.flags = FT_LOAD_MONOCHROME;
#endif

	//init backbuffer

		if(!(lbb = malloc(var_screeninfo.xres*var_screeninfo.yres)))
		{
			printf("Clock <allocating of Backbuffer failed>\n");
			FTC_Manager_Done(manager);
			FT_Done_FreeType(library);
			munmap(lfb, fix_screeninfo.smem_len);
			return -1;
		}

		memset(lbb, 0, var_screeninfo.xres*var_screeninfo.yres);

		startx = sx;
		starty = sy;
		mw=(big)?42:36;

	while(loop)
	{
		usleep(150000L);
		newmail=0;
		if(mail && ExistFile(MAIL_FILE))
		{
			if((tfh=fopen(MAIL_FILE,"r"))!=NULL)
			{
				if(fgets(tstr,511,tfh))
				{
					if(sscanf(tstr,"%d",&i))
					{
						newmail=i;
					}
				}
				fclose(tfh);
			}
		}
		ioctl(fb, FBIOGETCMAP, &colormap);
		found=0;
		trnsp=0;
		for(i=colormap.start;i<colormap.len && found!=7;i++)
		{
			if(!colormap.red[i] && !colormap.green[i] && !colormap.blue[i] && !colormap.transp[i])
			{
				cmc=i;
				found|=1;
			}
			if(colormap.red[i]>=0xF000 && colormap.green[i]>=0xF000  && colormap.blue[i]>=0xF000 && !colormap.transp[i])
			{
				cmct=i;
				found|=2;
			}
			if(colormap.transp[i]>trnsp)
			{
				trnspi=i;
				trnsp=colormap.transp[i];
				found|=4;
			}
		}	

		if(big)			//grosse Schrift (time/date)
		{
			margin_left = 3;			// 3
			digit_width = 14;			// 14
			margin_top_t = 26;			// 26
			font_size = BIG;			// 40
			x4 = 30;					// 30
			margin_top_d = 60;			// 60
		}
		else
		{
			margin_left = 7;			//7  Abstand links
			digit_width = 12;			//12 Ziffernblockbreite
			margin_top_t = 18;			//18 Abstand "Time" von oben
			font_size = MED;			//30 Schriftgroesse
			x4 = 18;					//18
			margin_top_d = 40;			//40 Abstand "Date" von oben
		}
		digits = 0;
		secs_width = 0;
		time(&atim);
		ltim = localtime(&atim);
		if (show_sec)
		{
			sprintf(tstr, "%02d:%02d:%02d", ltim->tm_hour, ltim->tm_min, ltim->tm_sec);
		}
		else
		{
			if (!blink)
				sprintf(tstr,"   %02d:%02d",ltim->tm_hour,ltim->tm_min);
			else
				sprintf(tstr,"   %02d%c%02d",ltim->tm_hour,(ltim->tm_sec & 1)?':':' ',ltim->tm_min);

			if (!show_date)
			{
				digits = 3;				//3 Platzhalter ':ss'
				secs_width = digits * digit_width;
			}		
		}
		if ((xpos >= mw) || (!show_sec))
		{
			ms = xpos + ((show_sec) ? 0 : 36 + 6 * big) - mw;	//mail left
		}
		else
		{
			ms = xpos + 100 + 20 * big;		//mail right
		}

		//paint Backgroundcolor to clear digits
		RenderBox(xpos+secs_width, ypos, xpos+secs_width+100+20*big, ypos+margin_top_t+2*(1+big), FILL, (bcol == COL_TRANSP)?trnspi:((bcol == COL_BLACK)?cmc:cmct));
		for(i=digits; i<strlen(tstr); i++)
		{
			*dstr=tstr[i];
			RenderString(dstr, xpos-margin_left+(i * digit_width), ypos+margin_top_t, 30, CENTER, font_size, (fcol == COL_TRANSP)?trnspi:((fcol == COL_WHITE)?cmct:cmc));
		}

		if(show_date)
		{
			sprintf(tstr,"%02d.%02d.%02d",ltim->tm_mday,ltim->tm_mon+1,ltim->tm_year-100);
			//Backgroundcolor Date
			RenderBox(xpos, ypos+x4, xpos+100+20*big, ypos+margin_top_d, FILL, (bcol == COL_TRANSP)?trnspi:((bcol == COL_BLACK)?cmc:cmct));
			for(i=0; i<strlen(tstr); i++)
			{
				*dstr=tstr[i];
				RenderString(dstr, xpos-margin_left+(i * digit_width), ypos+margin_top_d-2-(2*big), 30, CENTER, font_size, (fcol == COL_TRANSP)?trnspi:((fcol == COL_WHITE)?cmct:cmc));
			}
		}

		if(newmail > 0)
		{
			mailgfx = 1;

			//Background mail, left site from clock
			RenderBox(ms, ypos, ms+mw, ypos+margin_top_t+2*(1+big), FILL, (bcol == COL_TRANSP)?trnspi:((bcol == COL_BLACK)?cmc:cmct));
			if(!(ltim->tm_sec & 1))
			{
				RenderBox (ms+8, ypos+5+(1+big), ms+mw-8, ypos+margin_top_t+(1+big)-2, GRID, (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+8, ypos+5+(1+big), ms+mw-8, ypos+margin_top_t+(1+big)-2,       (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+8, ypos+margin_top_t+(1+big)-2, ms+mw-8, ypos+5+(1+big),       (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+(9+1*big), ypos+4+(1+big), ms+(mw/2), ypos+1,                  (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+(9+1*big), ypos+5+(1+big), ms+(mw/2), ypos+2,                  (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+(mw/2), ypos+1, ms+mw-(9+1*big), ypos+4+(1+big),               (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
				DrawLine  (ms+(mw/2), ypos+2, ms+mw-(9+1*big), ypos+5+(1+big),               (fcol == COL_TRANSP)?trnspi:((fcol == COL_BLACK)?cmc:cmct));
			}
			else
			{
				sprintf(tstr,"%d",newmail);
				RenderString(tstr, ms, ypos+margin_top_t, mw, CENTER, font_size, (fcol == COL_TRANSP)?trnspi:((fcol == COL_WHITE)?cmct:cmc));
			}
		}
		else
		{
			if (mailgfx > 0)
				RenderBox(ms, ypos, ms+mw, ypos+margin_top_t+2*(1+big), FILL, (!show_date || show_sec) ? trnspi : (bcol == COL_TRANSP) ? trnspi : ((bcol == COL_BLACK)?cmc:cmct));
			else
				ms=xpos;
		}

		w = 100 + 20 * big + ((mailgfx) ? ((show_sec) ? mw : 0) : - secs_width);
		for (i=0; i <= ((show_date) ? 20 : 10) * (2 + big); i++)
		{
			j = (starty + ypos + i) * var_screeninfo.xres + ( ((ms < xpos) ? ms : xpos) + ((show_sec) ? 0 : ((mailgfx) ? 0 : secs_width)) ) + startx;			
			if ((j+w) < var_screeninfo.xres * var_screeninfo.yres)
			{
				memcpy(lfb+j, lbb+j, w);
			}
		}
		if (newmail == 0 && mailgfx > 0)
			mailgfx = 0;

		if (++loop>5)
		{
			if(ExistFile("/tmp/.clock_kill"))
			{
				loop=0;
			}
		}
	}

	cmct=0;
	cmc=0;
	for(i=colormap.start;i<colormap.len;i++)
	{
		if(colormap.transp[i]>cmct)
		{
			cmc=i;
			cmct=colormap.transp[i];
		}
	}
	memset(lbb, cmc, var_screeninfo.xres*var_screeninfo.yres);	//cmc
	for (i=0; i <= ((show_date) ? 20 : 10) * (2 + big); i++)
	{
		j=(starty+ypos+i)*var_screeninfo.xres+((ms<xpos)?ms:xpos)+((show_sec)?0:((mailgfx)?0:secs_width))+startx;
		if((j+100+20*big+((mail)?mw:0))<var_screeninfo.xres*var_screeninfo.yres)
		{
			memcpy(lfb+j, lbb+j, w);
		}
	}
	FTC_Manager_Done(manager);
	FT_Done_FreeType(library);

	free(lbb);
	munmap(lfb, fix_screeninfo.smem_len);

	close(fb);
	remove("/tmp/.clock_kill");
	return 0;
}

