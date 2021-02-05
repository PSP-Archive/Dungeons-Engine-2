// textures.cpp

/*
Dungeons Engine 2

Copyright (C) 2008 Christoph Arnold "charnold" (http://www.caweb.de / charnold@gmx.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "textures.h"
#include "texture_formats.h"
#include "calog.h"
#include "plattform_includes.h"
#include "memory.h"
#include "vectorMath.h"
#include "paths.h"
#include <iostream>
using namespace std;

unsigned char cTextureTemp[512*512*4]; // temp location when loading texture

vector<mapTexture_t> mapTextures;

void loadMapTexture(const char *szFile, bool bMipmaps, int iPixelFormatT, bool bVRAM, float fRepeat, mapTexture_t::eTexAlignment alignment, bool bLightmapAllowed)
{
	mapTexture_t tmp;
	
	tmp.bMipmaps			= bMipmaps;
	tmp.bLightmapAllowed	= bLightmapAllowed;
	tmp.bVRAM				= bVRAM;
	tmp.iPixelFormatT		= iPixelFormatT;
	tmp.fRepeat				= fRepeat;
	tmp.alignment			= alignment;
	tmp.loadTextureFile(szFile);

	mapTextures.push_back(tmp);
}

void mapTexture_t::loadTextureFile(const char *szFile)
{
	strcpy(this->szFile, szFile);

	char szFullPath[256];

	sprintf(szFullPath, "%s/%s", TEXTURE_PATH, szFile);
	
	// TODO: Lightmaps anders laden !!! (die letzten beiden Parameter sind nur für Lightmaps, bzw das lmp file, das eigentlich keine lightmap ist, sondern eine normale textur für ein Licht...)
	
	iId = loadTexture(szFullPath, this->bMipmaps, this->iPixelFormatT, this->bVRAM, 0, 64, this->alignment);
}

void mapTexture_t::unload()
{
	unloadTexture(&iId);
}

void clearAllMapTextures()
{
	for (unsigned int i=mapTextures.size(); i>0; i--)
		mapTextures[i-1].unload();

	mapTextures.clear();
}

#if __APPLE_CC__ | _WIN32

void resetVRAM()
{
}

unsigned int loadTexture(const char *szFile, bool bMipmaps, int iPixelformatT, bool bVRAM, unsigned int *pLightmap, int iLightmapSize, mapTexture_t::eTexAlignment alignment)
{
	GLuint id = 0;
	int	width, height, size, success=0;
	
	unsigned char *pTexture;

	if( strstr( szFile, ".bmp" ) || strstr( szFile, ".BMP" ) )
	{
		success = loadBMP( szFile, cTextureTemp, &width, &height, true );
		pTexture = cTextureTemp;
	}
	else if( strstr( szFile, ".tga" ) || strstr( szFile, ".TGA" ) )
	{
		success = loadTGA( szFile, cTextureTemp, &width, &height, true );
		pTexture = cTextureTemp;
	}
	else if( strstr( szFile, ".lmp" ) || strstr( szFile, ".LMP" ) )
	{
		success = loadLMP( szFile, cTextureTemp, iLightmapSize );
		width = height = iLightmapSize;
		pTexture = cTextureTemp;
	}
	else if (pLightmap)
	{
		success = 1;
		width = height = iLightmapSize;
		pTexture = (unsigned char*)pLightmap;
	}

	if(success == 1)
	{
		size = (width < height ? width : height);

		// create and initialize new texture
		glGenTextures( 1, &id );
		glBindTexture( GL_TEXTURE_2D, id );
		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				
//		if (pLightmap || alignment == mapTexture_t::ALIGN_LOCAL)
		if (pLightmap)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}

		if (bMipmaps)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (bMipmaps)
			gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pTexture );
		else
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture );

		cout << szFile << " created with id " << id << endl;
	}
	else
	{
		cout << szFile << " failed !!!" << endl;
		writeLog("error loading texture:");
		writeLog(szFile);
	}
	
	return id;
}

void unloadTexture(unsigned int *id)
{
	glDeleteTextures( 1, (const GLuint*)id );
	cout << "texture id " << *id << " deleted." << endl;
}

int getTextureSize(unsigned int id)
{
	GLint val;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &val);
	return val;
}

void setCurrentTexture(unsigned int iTextureId)
{
	static unsigned int iTextureIdM = 0;
	static bool bStart = true;

	if (bStart || iTextureIdM != iTextureId)
	{
		bStart = false;
		iTextureIdM = iTextureId;
		glBindTexture(GL_TEXTURE_2D, iTextureId);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}

void printTextureInfo()
{
}

#elif _PSP

unsigned char cTextureTempUnswizzled[512*512*4]; // temp location when loading texture

struct Texture_t
{
	int size;
	unsigned char* bytes;
};

struct TextureObject_t
{
	int iTextures;
	int iPixelFormat;
	bool bSwizzled;
	bool bInVRam;
	Texture_t Texture[10];
	TextureObject_t() { iTextures=0; };
	void deleteObject();
};

#define VRAM_OFFSET (FRAME_SIZE*2 + ZBUF_SIZE)

#define MAX_PSP_TEXTURES 1024

TextureObject_t PSPTextures[MAX_PSP_TEXTURES];

// 2mb of vram	= 0x200000	= 2097152 Byte
// Framesize*2	= 0x110000	= 1114112 Byte	= 512*272*4*2
// Z-BufferSize	= 0x44000	=  278528 Byte	= 512*272*2
// Rest			= 0xAC000	=  704512 Byte	= 688KB

unsigned int vramaddr;

void TextureObject_t::deleteObject()
{
	if (!bInVRam)
	{
		for (int m=iTextures-1; m>=0; m--)
		{
			memoryFree(Texture[m].bytes);
		}
	}
	iTextures=0;
}

int genPSPTexId()
{
	for (int i=0; i<MAX_PSP_TEXTURES; i++)
	{
		if (!PSPTextures[i].iTextures)
			return i;
	}
	writeLog("Kein freies PSPTextures-Objekt gefunden !!!");
	return 0;
}

void make32to16(unsigned char *in32, unsigned char *out16, int size, int iPixelFormat)
{
	int iTexel;

	for (iTexel=0; iTexel<(size*size); iTexel++)
	{
		if (iPixelFormat == GU_PSM_4444)
		{
			*(out16)    = (*in32>>4) & 0x0f; in32++; // r
			*(out16++) |= (*in32)    & 0xf0; in32++; // g
			*(out16)    = (*in32>>4) & 0x0f; in32++; // b
			*(out16++) |= (*in32)    & 0xf0; in32++; // a
		}
		else if (iPixelFormat == GU_PSM_5650)
		{
			unsigned char r,g,b;

			r = (*in32>>3) & 0x1f; in32++;	// r = 5 bit
			g = (*in32>>2) & 0x3f; in32++;	// g = 6 bit
			b = (*in32>>3) & 0x1f; in32++;	// b = 5 bit
								   in32++;	// a = 0 bit

			*(out16)	= r;				// alle   5 bits von r auf lower  5 bits von out16
			*(out16++) |= (g<<5) & 0xe0;	// lower  3 bits von g auf higher 3 bits von out16
			*(out16)	= (g>>3) & 0x07;	// higher 3 bits von g auf lower  3 bits von out16
			*(out16++) |= (b<<3) & 0xf8;    // alle   5 bits von b auf higher 5 bits von out16

		}
		else if (iPixelFormat == GU_PSM_5551)
		{
			unsigned char r,g,b,a;

			r = (*in32>>3) & 0x1f; in32++;	// r = 5 bit
			g = (*in32>>3) & 0x1f; in32++;	// g = 5 bit
			b = (*in32>>3) & 0x1f; in32++;	// b = 5 bit
			a = (*in32>>7) & 0x01; in32++;	// a = 1 bit

			*(out16)	= r;				// alle   5 bits von r auf lower  5 bits von out16
			*(out16++) |= (g<<5) & 0xe0;	// lower  3 bits von g auf higher 3 bits von out16
			*(out16)	= (g>>3) & 0x03;	// higher 2 bits von g auf lower  2 bits von out16
			*(out16)   |= (b<<2) & 0x7c;    // alle   5 bits von b auf bits 3-7      von out16
			*(out16++) |= (a<<7) & 0x80;    //        1 bit  von a auf bit    8      von out16
		}
	}
}

void make32to32(unsigned char *in32, unsigned char *out32, int size)
{
	memcpy(out32, in32, (size*size*4));
}

void swizzle(unsigned char* out, unsigned char* in, unsigned int width, unsigned int height) 
{ 
   unsigned int i,j; 
   unsigned int rowblocks = (width / 16); 

   for (j = 0; j < height; ++j) 
   { 
      for (i = 0; i < width; ++i) 
      { 
         unsigned int blockx = i / 16; 
         unsigned int blocky = j / 8; 

         unsigned int x = (i - blockx*16); 
         unsigned int y = (j - blocky*8); 
         unsigned int block_index = blockx + ((blocky) * rowblocks); 
         unsigned int block_address = block_index * 16 * 8; 

         out[block_address + x + y * 16] = in[i+j*width]; 
      } 
   } 
}

// konvertiert von 32 auf 16 oder 32 bit, swizzelt + läd ins vram
unsigned char *convertImageVRAM1632(unsigned char *inptr,int size, int iPixelFormat)
{
	int bpp;
	unsigned char *output;
	size_t sizediff;

	if (iPixelFormat==GU_PSM_8888)
		bpp=4;
	else
		bpp=2;

	sizediff = size*size*bpp;

	// 1) allocate memory for output

	// make first vramaddr

	if (vramaddr == 0)
		vramaddr = (0x40000000 | 0x04000000) + VRAM_OFFSET;

	output = (unsigned char *)vramaddr;
	
	// 2) convert inptr -> cTextureTempUnswizzled

	if (iPixelFormat==GU_PSM_4444 || iPixelFormat==GU_PSM_5650 || iPixelFormat==GU_PSM_5551)
		make32to16(inptr, cTextureTempUnswizzled, size, iPixelFormat);
	else if (iPixelFormat==GU_PSM_8888)
		make32to32(inptr, cTextureTempUnswizzled, size);

	// 3) swizzle cTextureTempUnswizzled -> output

	swizzle(output, cTextureTempUnswizzled, (size*bpp), size);

	// make next vramaddr

	vramaddr += sizediff;

	if ((vramaddr & 0xff) != 0)
		vramaddr = (vramaddr & 0xffffff00) + 0x100;
	
	return output;
}

// konvertiert von 32 auf 16 oder 32 bit, swizzelt + läd ins ram
unsigned char *convertImageRAM1632(unsigned char *inptr,int size, int iPixelFormat)
{
	int bpp;
	unsigned char *output;
	size_t sizediff;

	if (iPixelFormat==GU_PSM_8888)
		bpp=4;
	else
		bpp=2;

	sizediff = size*size*bpp;

	// 1) allocate memory for output

	output = (unsigned char *)memoryAllocate(sizediff);

	// 2) convert inptr -> cTextureTempUnswizzled

	if (iPixelFormat==GU_PSM_4444 || iPixelFormat==GU_PSM_5650 || iPixelFormat==GU_PSM_5551)
		make32to16(inptr, cTextureTempUnswizzled, size, iPixelFormat);
	else if (iPixelFormat==GU_PSM_8888)
		make32to32(inptr, cTextureTempUnswizzled, size);
	
	// 3) swizzle cTextureTempUnswizzled -> output

	swizzle(output, cTextureTempUnswizzled, (size*bpp), size);

	return output;
}

void loadConvertTexture(int size, bool bVRAM, unsigned int iTexNum, int iMap, unsigned char *buffer)
{
	PSPTextures[iTexNum].iTextures++;
	PSPTextures[iTexNum].Texture[iMap].size = size;
	if (bVRAM)
	{
		PSPTextures[iTexNum].Texture[iMap].bytes = convertImageVRAM1632(buffer, size, PSPTextures[iTexNum].iPixelFormat);
		PSPTextures[iTexNum].bSwizzled = true;
	}
	else
	{
		PSPTextures[iTexNum].Texture[iMap].bytes = convertImageRAM1632(buffer, size, PSPTextures[iTexNum].iPixelFormat);
		PSPTextures[iTexNum].bSwizzled = true;
	}
}

void makeMipMaps(unsigned char *buffer, int sourcesize, int iTotalTextures, unsigned int iTexNum, bool bVRAM)
{
	int destsize, pix1, pix2, pix3, pix4, linecheck, base, iMapsToCreate=iTotalTextures;

	// MipMap level 0 speichern
	loadConvertTexture(sourcesize, bVRAM, iTexNum, (iTotalTextures-iMapsToCreate), buffer);
	iMapsToCreate--;

	while ( iMapsToCreate > 0 )
	{
		destsize = (sourcesize/2);
		base=0; linecheck=0;
		for (int currpix=0; currpix<(destsize*destsize); currpix++)
		{
			if (linecheck==destsize)
			{
				// neue destline -> eine weitere source-pixelline überspringen
				base+=sourcesize;
				linecheck=0;
			}
			pix1=(base+(currpix*2));
			pix2=(pix1+1);
			pix3=(pix1+sourcesize);
			pix4=(pix3+1);

			int bpp=4;

			for (int i=0; i<4; i++)
			{
				buffer[currpix*bpp+i]=(unsigned char)((buffer[pix1*bpp+i]+buffer[pix2*bpp+i]+buffer[pix3*bpp+i]+buffer[pix4*bpp+i])/4);
			}

			linecheck++;
		}
		
		loadConvertTexture(destsize, bVRAM, iTexNum, (iTotalTextures-iMapsToCreate), buffer);
		iMapsToCreate--;

		sourcesize = destsize;
		if (sourcesize==1) iMapsToCreate=0;
	}
}

void resetVRAM()
{
	vramaddr = 0;
}

unsigned int loadTexture(const char *szFile, bool bMipmaps, int iPixelFormatT, bool bVRAM, unsigned int *pLightmap, int iLightmapSize, mapTexture_t::eTexAlignment alignment)
{
	unsigned int iTexNum = genPSPTexId();
	int success = 0, width = 0, height = 0, size, iPixelformat;
	bool flipvert = true;
	unsigned char *pTexture;

	switch (iPixelFormatT)
	{
		case 4444:
			iPixelformat = GU_PSM_4444;
			break;
		case 5551:
			iPixelformat = GU_PSM_5551;
			break;
		case 5650:
			iPixelformat = GU_PSM_5650;
			break;
		case 8888:
			iPixelformat = GU_PSM_8888;
			break;
		default:
			iPixelformat = GU_PSM_5650;
	}

	PSPTextures[iTexNum].iPixelFormat	= iPixelformat;
	PSPTextures[iTexNum].iTextures		= 0;
	PSPTextures[iTexNum].bInVRam		= bVRAM;

	if ( strstr( szFile, ".bmp" ) || strstr( szFile, ".BMP" ) )
	{
		success = loadBMP( szFile, cTextureTemp, &width, &height, flipvert );
		size = (width < height ? width : height);
		pTexture = cTextureTemp;
	}
	else if ( strstr( szFile, ".tga" ) || strstr( szFile, ".TGA" ) )
	{
		success = loadTGA( szFile, cTextureTemp, &width, &height, flipvert );
		size = (width < height ? width : height);
		pTexture = cTextureTemp;
	}
	else if( strstr( szFile, ".lmp" ) || strstr( szFile, ".LMP" ) )
	{
		success = loadLMP( szFile, cTextureTemp, iLightmapSize );
		size = iLightmapSize;
		pTexture = cTextureTemp;
	}
	else if (pLightmap)
	{
		success = 1;
		size = iLightmapSize;
		pTexture = (unsigned char*)pLightmap;
	}

	if (success == 1)
	{
		int iTotalTextures ;

		if (!bMipmaps)
		{
			iTotalTextures = 1;
		}
		else
		{
			// mipmaps erzeugen (wegen swizzle nur bis 16x16)
			if		(size >= 512) iTotalTextures = 6;
			else if	(size >= 256) iTotalTextures = 5;
			else if (size >= 128) iTotalTextures = 4;
			else if (size >=  64) iTotalTextures = 3;
			else if (size >=  32) iTotalTextures = 2;
			else				  iTotalTextures = 1;
		}

		// MipMap levels erzeugen
		makeMipMaps(pTexture, size, iTotalTextures, iTexNum, bVRAM);
	}
	else
	{
		writeLog("error loading texture:");
		writeLog(szFile);
	}

	return iTexNum;
}

void unloadTexture(unsigned int *id)
{
	PSPTextures[*id].deleteObject();
}

int getTextureSize(unsigned int id)
{
	return PSPTextures[id].Texture[0].size;
}

void setCurrentTexture(unsigned int iTextureId)
{
	static unsigned int iTextureIdM = 0;
	static bool bStart = true;

	if (bStart || iTextureIdM != iTextureId)
	{
		bStart = false;
		iTextureIdM = iTextureId;
		int iTextures = PSPTextures[iTextureId].iTextures;

		if (iTextures > 0)
		{
			sceGuTexMode(PSPTextures[iTextureId].iPixelFormat,iTextures-1,0,PSPTextures[iTextureId].bSwizzled);
			for (int i=0; i<iTextures; i++)
			{
				int iSize;
				iSize = PSPTextures[iTextureId].Texture[i].size;
				sceGuTexImage(i,iSize,iSize,iSize,PSPTextures[iTextureId].Texture[i].bytes);
			}
		}
	}
}

void printTextureInfo()
{
	char text[200];
	for (int i=0; i<MAX_PSP_TEXTURES; i++)
	{
		if (PSPTextures[i].iTextures)
		{
			sprintf(text, "Texture %i: %i Texturemaps", i, PSPTextures[i].iTextures);
			writeLog(text);
			for (int j=0; j<PSPTextures[i].iTextures; j++)
			{
				sprintf(text, "        Map %i: Size = %i", j, PSPTextures[i].Texture[j].size);
				writeLog(text);
			}
		}
		else
		{
			//sprintf(text, "Texture %i has no Texturemaps", i);
			//writeLog(text);
		}
	}
}

#endif

vector<textureAnimation_t> textureAnimations;

void clearAllTextureAnimations()
{
	for (int i=textureAnimations.size(); i>0; i--)
	{
		textureAnimations[i-1].frameTextureMapIndex.clear();
	}

	textureAnimations.clear();
}

namespace textureEffects
{
	float fMovementValue[4];
	float fBlendValue[5];
	float c[5];
	float fAim[5];

	void initAnimations()
	{
		int iAnimations = textureAnimations.size();
		
		for (int iAnim = 0; iAnim < iAnimations; iAnim++)
		{
			textureAnimations[iAnim].iRunningFrameIndex = 0;
			textureAnimations[iAnim].fRunningDelay = 0.0f;
		}
	}
	
	void init()
	{
		for (int i=0; i<4; i++)
			fMovementValue[i] = 0;

		for (int i=0; i<5; i++)
			fBlendValue[i] = 0;
			
		for (int i=0; i<5; i++)
			c[i] = 0;
			
		for (int i=0; i<2; i++)
			fAim[i] = 1;
			
		initAnimations();
	}

	void advanceAnimations(float fElapsedTime)
	{
		int iAnimations = textureAnimations.size();
		
		for (int iAnim = 0; iAnim < iAnimations; iAnim++)
		{
			textureAnimations[iAnim].fRunningDelay += fElapsedTime;
			
			while (textureAnimations[iAnim].fRunningDelay > textureAnimations[iAnim].fFrameDelay)
			{
				textureAnimation_t *pAnim = &textureAnimations[iAnim];
				
				pAnim->fRunningDelay -= pAnim->fFrameDelay;
				pAnim->iRunningFrameIndex++;
				if (pAnim->iRunningFrameIndex >= pAnim->frameTextureMapIndex.size()) pAnim->iRunningFrameIndex = 0;
			}
		}
	}

	void advance(float fElapsedTime)
	{
		fMovementValue[0] += fElapsedTime*1.0f;
		while (fMovementValue[0]>1.0f) fMovementValue[0]-=1.0f;

		fMovementValue[1] -= fElapsedTime*1.0f;
		while (fMovementValue[1]<-1.0f) fMovementValue[1]+=1.0f;

		fMovementValue[2] += fElapsedTime*2.0f;
		while (fMovementValue[2]>1.0f) fMovementValue[2]-=1.0f;

		fMovementValue[3] -= fElapsedTime*2.0f;
		while (fMovementValue[3]<-1.0f) fMovementValue[3]+=1.0f;
		
		c[0] += fElapsedTime*4.0f;
		c[1] += fElapsedTime*10.0f;
		c[2] += fElapsedTime*80.0f;
		c[3] += fElapsedTime*0.4f*fAim[0];
		c[4] += fElapsedTime*1.0f*fAim[1];

		if (c[3] > 1) { fAim[0] *= -1; c[3] = 1; }
		if (c[3] < 0) {	fAim[0] *= -1; c[3] = 0; }
		if (c[4] > 1) { fAim[1] *= -1; c[4] = 1; }
		if (c[4] < 0) {	fAim[1] *= -1; c[4] = 0; }

		fBlendValue[0]=(sinfCa(c[0])+1)*0.5f;
		fBlendValue[1]=(sinfCa(c[1])+1)*0.5f;
		fBlendValue[2]=(sinfCa(c[2])+1)*0.5f;
		fBlendValue[3]=c[3];
		fBlendValue[4]=c[4];
		
		advanceAnimations(fElapsedTime);
	}
};
