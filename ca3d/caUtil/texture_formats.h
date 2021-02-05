// texture_formats.h

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

#ifndef CA_TEXTURE_FORMATS_H
#define CA_TEXTURE_FORMATS_H

// magic number "BM"
#define			BITMAP_IDX			('B' + ('M'<<8))

// header byte type for RLE
#define			RLE_COMMANDX		0
#define			RLE_ENDOFLINEX		0
#define			RLE_ENDOFBITMAPX	1
#define			RLE_DELTAX			2
#define			BI_OS2X				-1

// compression type
#define			BI_RGBX				0
#define			BI_RLE8X			1
#define			BI_RLE4X			2
#define			BI_BITFIELDSX		3

//#pragma warning( disable : 4103 )

// --------------------------------------------
// tagBITMAPFILEHEADER - bitmap file header.
// --------------------------------------------

#pragma pack(2)

typedef struct tagBITMAPFILEHEADERX		// bmfh
{
	unsigned short	bfType;				// magic number "BM"
	unsigned int	bfSize;				// file size
	unsigned short	bfReserved1;		// reserved
	unsigned short	bfReserved2;		// reserved
	unsigned int	bfOffBits;			// offset to bitmap data

} BITMAPFILEHEADERX, *PBITMAPFILEHEADERX;

#pragma pack()

// --------------------------------------------
// tagBITMAPCOREHEADER - bitmap core header.
// --------------------------------------------

typedef struct tagBITMAPCOREHEADERX		// bmch
{
	unsigned int	bcSize;				// size of the structure
	unsigned short	bcWidth;			// image width
	unsigned short	bcHeight;			// image height
	unsigned short	bcPlanes;			// must be equal to 1
	unsigned short	bcBitCount;			// number of bits per pixel

} BITMAPCOREHEADERX, *PBITMAPCOREHEADERX;

// --------------------------------------------
// tagRGBTRIPLE - 24 bits pixel
// --------------------------------------------

#pragma pack(1)

typedef struct tagRGBTRIPLEX			// rgbt
{
	unsigned char	rgbtBlue;		// blue
	unsigned char	rgbtGreen;		// green
	unsigned char	rgbtRed;		// red

} RGBTRIPLEX, *PRGBTRIPLEX;

#pragma pack()

// --------------------------------------------
// tagRGBQUAD - 32 bits pixel
// --------------------------------------------

typedef struct tagRGBQUADX			// rgbt
{
	unsigned char	rgbBlue;		// blue
	unsigned char	rgbGreen;		// green
	unsigned char	rgbRed;			// red
	unsigned char	rgbReserved;	// reserved

} RGBQUADX, *PRGBQUADX;

// --------------------------------------------
// tagBITMAPCOREINFO - bitmap core info.
// --------------------------------------------

typedef struct tagBITMAPCOREINFOX		// bmci
{
	BITMAPCOREHEADERX	bmciHeader;		// size of the structure
	RGBTRIPLEX			bcmiColors[1];	// color palette

} BITMAPCOREINFOX, *PBITMAPCOREINFOX;

// --------------------------------------------
// BITMAPFILEHEADER - bitmap info header.
// --------------------------------------------

typedef struct tagBITMAPINFOHEADERX
{
	unsigned int	biSize;				// size of the structure
	int				biWidth;			// image width
	int				biHeight;			// image height
	unsigned short	biPlanes;			// must be equal to 1
	unsigned short	biBitCount;			// number of bits per pixel
	unsigned int	biCompression;		// compression type
	unsigned int	biSizeImage;		// size of data bitmap
	int				biXPelsPerMeter;	// number of pixels per meter on the X axis
	int				biYPelsPerMeter;	// number of pixels per meter on the Y axis
	unsigned int	biClrUsed;			// number of colors used
	unsigned int	biClrImportant;		// number of important colors

} BITMAPINFOHEADERX, *PBITMAPINFOHEADERX;

// prototype
int loadBMP( const char *filename, unsigned char *pixels, int *width, int *height, bool flipvert );






/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

// --------------------------------------------
// TGAHEADER - targa header.
// --------------------------------------------

#pragma pack(1)

typedef struct tagTGAHEADER
{
	unsigned char	id_lenght;			// size of the structure
	unsigned char	color_map_type;		// must be equal to 0
	unsigned char	image_type;			// image compression type

	short int		cm_first_entry;		// colormap_origin (toujours 0)
	short int		cm_length;			// colormap_length (toujours 0)
	unsigned char	cm_size;			// colormap_size (toujours 0)

	short int		is_xorigin;			// lower left X coordinate (always 0)
	short int		is_yorigin;			// lower left Y coordinate (always 0)

	short int		is_width;			// image width (in pixels)
	short int		is_height;			// image height (in pixels)

	unsigned char	is_pixel_depth;		// number of bits per pixel: 16, 24, 32
	unsigned char	is_image_descriptor;// 24 bits = 0x00; 32 bits = 0x80

} TGAHEADER, *PTGAHEADER;

#pragma pack()

//#ifdef WIN32
//#include	<windows.h>
//#else

// --------------------------------------------
// RGBTRIPLE - 24 bits pixel
// --------------------------------------------

typedef struct tagRGBTRIPLEY			// rgbt
{
	unsigned char	rgbtBlue;		// blue
	unsigned char	rgbtGreen;		// green
	unsigned char	rgbtRed;		// red

} RGBTRIPLEY, *PRGBTRIPLEY;

//#endif	// WIN32

// --------------------------------------------
// BGRAQUAD - 32 bits pixel
// --------------------------------------------

typedef struct tagBGRAQUAD			// rgbt
{
	unsigned char	bgraBlue;		// blue
	unsigned char	bgraGreen;		// green
	unsigned char	bgraRed;		// red
	unsigned char	bgraAlpha;		// alpha

} BGRAQUAD, *PBGRAQUAD;

// prototype
int loadTGA( const char *filename, unsigned char *pixels, int *width, int *height, bool flipvert );






// prototype
int loadLMP( const char *filename, unsigned char *pixels, int iLightmapsize );

#endif
