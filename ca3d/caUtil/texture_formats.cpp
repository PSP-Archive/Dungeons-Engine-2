// texture_formats.cpp

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

#include "texture_formats.h"
#include "memory.h"
#include <fstream>
using namespace std;

// ---------------------------------------------------
// loadBMP() - load a Windows/OS2 BITMAP image [.bmp].
//
// parameters :
//    - filename [in]  : image source file
//    - pixels	 [out] : 32 bits rgba image data
//    - width    [out] : image width in pixels
//    - height   [out] : image height in pixels
//    - flipvert [in]  : flip vertically
//
// return value :
//    - -1 : no image data
//    -  0 : failure
//    -  1 : success
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// accepted image formats :
//     # RGB 1-4-8-24-32 bits	WINDOWS - OS/2
//     # RLE 4-8 bits			WINDOWS
// --------------------------------------------------

int loadBMP( const char *filename, unsigned char *pixelsin, int *width, int *height, bool flipvert )
{
	BITMAPFILEHEADERX	sbmfh;			// bitmap file header
	BITMAPINFOHEADERX	sbmih;			// bitmap info header (windows)
	//BITMAPCOREHEADERX	sbmch;			// bitmap core header (os/2)

	BITMAPFILEHEADERX	*bmfh;			// bitmap file header
	BITMAPINFOHEADERX	*bmih;			// bitmap info header (windows)
	//BITMAPCOREHEADERX	*bmch;			// bitmap core header (os/2)

	RGBTRIPLEX			*os2_palette;	// pointer to the color palette os/2
	RGBQUADX			*win_palette;	// pointer to the color palette windows
	char				*buffer;		// buffer storing the entire file
	unsigned char		*ptr;			// pointer to pixels data
	int					bitCount;		// number of bits per pixel
	int					compression;	// compression type (rgb/rle)
	int					row, col, i;	// temporary variables
	int					w, h;			// width, height
	std::ifstream		file;			// file stream
	
	unsigned char **pixels = &pixelsin;

	unsigned char trans_blue_lim = 250;

	/////////////////////////////////////////////////////
	// read the entire file in the buffer

	file.open( filename, std::ios::in | std::ios::binary );
	
	if( file.fail() )
		return 0;

	file.seekg( 0, std::ios::end );
	long flen = file.tellg();
	file.seekg( 0, std::ios::beg );
	//buffer = new char[ flen + 1 ];
	buffer = (char*)memoryAllocate(sizeof(char)*(flen + 1));
	file.read( buffer, flen );

	file.seekg( 0, std::ios::beg );
	file.read( (char*)&sbmfh, sizeof(BITMAPFILEHEADERX) );
	file.read( (char*)&sbmih, sizeof(BITMAPINFOHEADERX) );

	//file.seekg( 0, std::ios::beg );
	//file.read( (char*)&sbmfh, sizeof(BITMAPFILEHEADERX) );
	//file.read( (char*)&sbmch, sizeof(BITMAPCOREHEADERX) );

	bmfh = &sbmfh;
	bmih = &sbmih;
	//bmch = &sbmch;

	char *pBuff = buffer;

	file.clear();
	file.close();

	/////////////////////////////////////////////////////
	// read the header

	pBuff += sizeof( BITMAPFILEHEADERX );

	// verify that it's a BITMAP file
	if( bmfh->bfType != BITMAP_IDX )
	{
		//delete [] buffer;
		memoryFree(buffer);
		return 0;
	}
	
	/*
	if ( (bmih->biCompression < 0) || (bmih->biCompression > 3) )
	{
		// OS/2 style
		pBuff += sizeof( BITMAPCOREHEADERX );

		bitCount	= bmch->bcBitCount;
		compression	= BI_OS2X;

		w = bmch->bcWidth;
		h = bmch->bcHeight;
	}
	else
	*/
	{
		// WINDOWS style
		pBuff += sizeof( BITMAPINFOHEADERX );

		bitCount	= bmih->biBitCount;
		compression	= bmih->biCompression;

		w = bmih->biWidth;
		h = bmih->biHeight;
	}

	if( width )
		*width	= w;

	if( height )
		*height	= h;

	if( !pixels )
	{
		//delete [] buffer;
		memoryFree(buffer);
		return (-1);
	}

	/////////////////////////////////////////////////////
	// read the palette

	if( bitCount <= 8 )
	{
		// 24 and 32 bits images are not paletted

		// ajust the palette pointer to the memory in the buffer
		os2_palette = (RGBTRIPLEX *)pBuff;
		win_palette = (RGBQUADX *)pBuff;

		//	[number of colors in the palette] * [size of one pixel]
		pBuff += (1 << bitCount) * (bitCount >> 3) * sizeof( unsigned char );
	}

	/////////////////////////////////////////////////////
	// allocate memory to store pixel data

//	*pixels = new unsigned char[ w * h * 4 ];
	ptr		= &(*pixels)[0];

	// move the pixel data pointer to the begening of bitmap data
	pBuff = buffer + (bmfh->bfOffBits * sizeof( char ));

	/////////////////////////////////////////////////////
	// read pixel data following the image compression
	// type and the number of bits per pixels
	/////////////////////////////////////////////////////

	switch( compression )
	{
		case BI_OS2X:
		case BI_RGBX:
		{
			for( row = h - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * w * 4 ];

				switch( bitCount )
				{
					case 1:
					{
// RGB 1 BITS
						for( col = 0; col < (int)(w / 8); col++ )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							for( i = 7; i >= 0; i--, ptr += 4 )
							{
								// convert indexed pixel (1 bit) into rgba (32 bits) pixel
								int clrIdx = ((color & (1<<i)) > 0);

								if( compression == BI_OS2X )
								{
									ptr[0] = os2_palette[ clrIdx ].rgbtRed;
									ptr[1] = os2_palette[ clrIdx ].rgbtGreen;
									ptr[2] = os2_palette[ clrIdx ].rgbtBlue;
									if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

										// switch color to black 12.06.2006
										ptr[0] = 0;
										ptr[1] = 0;
										ptr[2] = 0;

										// transparent
										ptr[3] = 0;
									}
									else ptr[3] = 255;
								}
								else
								{
									ptr[0] = win_palette[ clrIdx ].rgbRed;
									ptr[1] = win_palette[ clrIdx ].rgbGreen;
									ptr[2] = win_palette[ clrIdx ].rgbBlue;
									if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

										// switch color to black 12.06.2006
										ptr[0] = 0;
										ptr[1] = 0;
										ptr[2] = 0;

										// transparent
										ptr[3] = 0;
									}
									else ptr[3] = 255;
								}
							}
						}

						break;
					}

					case 4:
					{
// RGB 4 BITS
						for( col = 0; col < (int)(w / 2); col++, ptr += 8 )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							// convert indexed pixel (4 bits) into rgba (32 bits) pixel
							int clrIdx;

							if( compression == BI_OS2X )
							{
								clrIdx = (color >> 4);
								ptr[0] = os2_palette[ clrIdx ].rgbtRed;
								ptr[1] = os2_palette[ clrIdx ].rgbtGreen;
								ptr[2] = os2_palette[ clrIdx ].rgbtBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;

								clrIdx = (color & 0x0F);
								ptr[4] = os2_palette[ clrIdx ].rgbtRed;
								ptr[5] = os2_palette[ clrIdx ].rgbtGreen;
								ptr[6] = os2_palette[ clrIdx ].rgbtBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;
							}
							else
							{
								clrIdx = (color >> 4);
								ptr[0] = win_palette[ clrIdx ].rgbRed;
								ptr[1] = win_palette[ clrIdx ].rgbGreen;
								ptr[2] = win_palette[ clrIdx ].rgbBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;

								clrIdx = (color & 0x0F);
								ptr[4] = win_palette[ clrIdx ].rgbRed;
								ptr[5] = win_palette[ clrIdx ].rgbGreen;
								ptr[6] = win_palette[ clrIdx ].rgbBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;
							}
						}

						break;
					}

					case 8:
					{
// RGB 8 BITS
						for( col = 0; col < w; col++, ptr += 4 )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							// convert indexed pixel (8 bits) into rgba (32 bits) pixel
							if( compression == BI_OS2X )
							{
								ptr[0] = os2_palette[ color ].rgbtRed;
								ptr[1] = os2_palette[ color ].rgbtGreen;
								ptr[2] = os2_palette[ color ].rgbtBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;
							}
							else
							{
								ptr[0] = win_palette[ color ].rgbRed;
								ptr[1] = win_palette[ color ].rgbGreen;
								ptr[2] = win_palette[ color ].rgbBlue;
								if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

									// switch color to black 12.06.2006
									ptr[0] = 0;
									ptr[1] = 0;
									ptr[2] = 0;

									// transparent
									ptr[3] = 0;
								}
								else ptr[3] = 255;
							}
						}

						break;
					}

					case 24:
					{
// RGB 24 BITS
						for( col = 0; col < w; col++, ptr += 4 )
						{
							// convert bgr pixel (24 bits) into rgba (32 bits) pixel
							RGBTRIPLEX *pix = (RGBTRIPLEX *)pBuff;
							pBuff += sizeof( RGBTRIPLEX );

							ptr[0] = pix->rgbtRed;
							ptr[1] = pix->rgbtGreen;
							ptr[2] = pix->rgbtBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}

  						break;
					}

					case 32:
					{
// RGB 32 BITS
						for( col = 0; col < w; col++, ptr += 4 )
						{
							// // convert bgr pixel (32 bits) into rgba (32 bits) pixel
							RGBQUADX *pix = (RGBQUADX *)pBuff;
							pBuff += sizeof( RGBQUADX );

							ptr[0] = pix->rgbRed;
							ptr[1] = pix->rgbGreen;
							ptr[2] = pix->rgbBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}

						break;
					}
				}
			}

			break;
		}

		case BI_RLE8X:
		{
// RLE 8 BITS
			for( row = h - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * w * 4 ];

				for( col = 0; col < w; /* nothing */ )
				{
					// get one packet (2 bytes)
					unsigned char byte1 = *((unsigned char *)(pBuff++));
					unsigned char byte2 = *((unsigned char *)(pBuff++));


					if( byte1 == RLE_COMMANDX )
					{
						// absolute encoding
						for( i = 0; i < byte2; i++, ptr += 4, col++ )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							// convert indexed pixel (8 bits) into rgba (32 bits) pixel
							ptr[0] = win_palette[ color ].rgbRed;
							ptr[1] = win_palette[ color ].rgbGreen;
							ptr[2] = win_palette[ color ].rgbBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}

						if( (byte2 % 2) == 1 )
							pBuff++;
					}
					else
					{
						// read next pixels
						for( i = 0; i < byte1; i++, ptr += 4, col++ )
						{
							// convert indexed pixel (8 bits) into rgba (32 bits) pixel
							ptr[0] = win_palette[ byte2 ].rgbRed;
							ptr[1] = win_palette[ byte2 ].rgbGreen;
							ptr[2] = win_palette[ byte2 ].rgbBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}        
					}
				}
			}

			break;
		}

		case BI_RLE4X:
		{
// RLE 4 BITS
			unsigned char color;
			int	bytesRead = 0;		// number of bytes read

			for( row = h - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * w * 4 ];

				for( col = 0; col < w; /* nothing */ )
				{
					// get one packet (2 bytes)
					unsigned char byte1 = *((unsigned char *)(pBuff++));
					unsigned char byte2 = *((unsigned char *)(pBuff++));

					bytesRead += 2;

					if( byte1 == RLE_COMMANDX )
					{
						// absolute encoding
						unsigned char databyte;

						for( i = 0; i < byte2; i++, ptr += 4, col++ )
						{
							if( (i % 2) == 0 )
							{
								// read the current pixel
								databyte = *((unsigned char *)(pBuff++));
								bytesRead++;

								color = (databyte >> 4);	// 4 first bits
							}
							else
							{
								color = (databyte & 0x0F);	// 4 last bits
							}

							// convert indexed pixel (4 bits) into rgba (32 bits) pixel
							ptr[0] = win_palette[ color ].rgbRed;
							ptr[1] = win_palette[ color ].rgbGreen;
							ptr[2] = win_palette[ color ].rgbBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}
          
						while( (bytesRead % 2) != 0 )
						{
							pBuff++;
							bytesRead++;
						}
					}
					else
					{
						// read next pixels
						for( i = 0; i < byte1; i++, ptr += 4, col++ )
						{
							if( (i % 2) == 0 )
								color = (byte2 >> 4);	// 4 first bits
							else
								color = (byte2 & 0x0F);	// 4 last bits

							// convert indexed pixel (4 bits) into rgba (32 bits) pixel
							ptr[0] = win_palette[ color ].rgbRed;
							ptr[1] = win_palette[ color ].rgbGreen;
							ptr[2] = win_palette[ color ].rgbBlue;
							if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] > trans_blue_lim) {

								// switch color to black 12.06.2006
								ptr[0] = 0;
								ptr[1] = 0;
								ptr[2] = 0;

								// transparent
								ptr[3] = 0;
							}
							else ptr[3] = 255;
						}        
					}
				}
			}

			break;
		}
	}

	// free buffer memory
	//delete [] buffer;
	memoryFree(buffer);

	// return success
	return 1;
}












// --------------------------------------------------
// loadTGA() - load a TrueVision TARGA image [.tga].
//
// parameters :
//    - filename [in]  : image source file
//    - pixels	 [out] : 32 bits rgba image data
//    - width    [out] : image width in pixels
//    - height   [out] : image height in pixels
//    - flipvert [in]  : flip vertically
//
// return value :
//    - -1 : no image data
//    -  0 : failure
//    -  1 : success
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// accepted image formats :
//     # RGB 8-16-24-32 bits
//     # RLE 8-16-24-32 bits
// --------------------------------------------------

int loadTGA( const char *filename, unsigned char *pixelsin, int *width, int *height, bool flipvert )
{
	std::ifstream	file;			// file stream
	TGAHEADER		*tgah;			// targa header
	RGBTRIPLEY		*palette;		// pointer on the color palette
	char			*buffer;		// buffer storing the entire file
	unsigned char	*ptr;			// pointer to pixels data
	int				row, col, i;	// temporary variables

	unsigned char **pixels = &pixelsin;


	/////////////////////////////////////////////////////
	// read the entire file in the buffer

	file.open( filename, std::ios::in | std::ios::binary );

	if( file.fail() )
		return 0;

	file.seekg( 0, std::ios::end );
	long flen = file.tellg();
	file.seekg( 0, std::ios::beg );

	//buffer = new char[ flen + 1 ];
	buffer = (char*)memoryAllocate(sizeof(char) * (flen + 1));
	file.read( buffer, flen );
	char *pBuff = buffer;

	file.clear();
	file.close();

	/////////////////////////////////////////////////////
	// read the header

	tgah = (TGAHEADER *)pBuff;
	pBuff += sizeof( TGAHEADER );

	if( width )
		*width = tgah->is_width;

	if( height )
		*height = tgah->is_height;

	if( !pixels )
	{
		//delete [] buffer;
		memoryFree(buffer);
		return (-1);
	}

	ptr		= &(*pixels)[0];

	// move the pixel data pointer to the begening of bitmap data
	if( tgah->id_lenght )
		pBuff = buffer + (tgah->id_lenght * sizeof( unsigned char ));

	/////////////////////////////////////////////////////
	// read the palette

	if( tgah->color_map_type )
	{
		// 24 and 32 bits images are not paletted

		// ajust the palette pointer to the memory in the buffer
		palette = (RGBTRIPLEY *)pBuff;

		pBuff += tgah->cm_length * (tgah->cm_size >> 3) * sizeof( unsigned char );
	}

	/////////////////////////////////////////////////////


	/////////////////////////////////////////////////////
	// read pixel data following the image compression
	// type and the number of bits per pixels
	/////////////////////////////////////////////////////

	switch( tgah->image_type )
	{
		case 0:
			// pas de données image
			break;

		case 1:
// COLOR-MAPPED BGR 8 BITS GREYSCALE

		case 3:
		{
// COLOR-MAPPED BGR 8 BITS
			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; col++, ptr += 4 )
				{
					// read the current pixel
					unsigned char color = *((unsigned char *)(pBuff++));

					// convert indexed pixel (8 bits) into rgba (32 bits) pixel
					ptr[0] = palette[ color ].rgbtRed;		// b->r
					ptr[1] = palette[ color ].rgbtGreen;	// g->g
					ptr[2] = palette[ color ].rgbtBlue;		// r->b
					ptr[3] = 255;							// alpha
				}
			}

			break;
		}

		case 2:
		{
			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; col++, ptr += 4 )
				{
					switch( tgah->is_pixel_depth )
					{
						case 16:
						{
// TRUE-COLOR BGR 16 BITS
							// read the current pixel
							unsigned short color = *((unsigned short *)pBuff);
							pBuff += sizeof( short );

							// convert bgr (16 bits) pixel into rgba (32 bits) pixel
							ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
							ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
							ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
							ptr[3] = 255;							// alpha

							break;
						}

						case 24:
						{
// TRUE-COLOR BGR 24 BITS
							// convert bgr (24 bits) pixel into rgba (32 bits) pixel
							RGBTRIPLEY *pix = (RGBTRIPLEY *)pBuff;
							pBuff += sizeof( RGBTRIPLEY );

							ptr[0] = pix->rgbtRed;
							ptr[1] = pix->rgbtGreen;
							ptr[2] = pix->rgbtBlue;
							ptr[3] = 255;

							break;
						}

						case 32:
						{
// TRUE-COLOR BGR 32 BITS
							// convert bgr (32 bits) pixel into rgba (32 bits) pixel
							BGRAQUAD *pix = (BGRAQUAD *)pBuff;
							pBuff += sizeof( BGRAQUAD );

							ptr[0] = pix->bgraRed;
							ptr[1] = pix->bgraGreen;
							ptr[2] = pix->bgraBlue;
							ptr[3] = pix->bgraAlpha;

							break;
						}
					}
				}
			}

			break;
		}

		case 9:
// RLE COLOR-MAPPED BGR 8 BITS

		case 11:
		{
// RLE COLOR-MAPPED BGR 8 BITS GREYSCALE

			unsigned char	packetHeader, packetSize, i;


			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; /* rien */ )
				{
					packetHeader	= *((unsigned char *)(pBuff++));
					packetSize		= 1 + (packetHeader & 0x7f);

					if( packetHeader & 0x80 )
					{
						// run-length packet

						// read the current pixel
						unsigned char color = *((unsigned char *)(pBuff++));

						// convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							ptr[0] = palette[ color ].rgbtRed;		// b->r
							ptr[1] = palette[ color ].rgbtGreen;	// g->g
							ptr[2] = palette[ color ].rgbtBlue;		// r->b
							ptr[3] = 255;							// alpha
						}
					}
					else
					{
						// non run-length packet

						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							// read the current pixel
							unsigned char color = *((unsigned char *)(pBuff++));

							// convert indexed pixel (8 bits) pixel into rgba (32 bits) pixel
							ptr[0] = palette[ color ].rgbtRed;		// b->r
							ptr[1] = palette[ color ].rgbtGreen;	// g->g
							ptr[2] = palette[ color ].rgbtBlue;		// r->b
							ptr[3] = 255;							// alpha
						}
					}
				}
			}

			break;
		}

		case 10:
		{
			unsigned char	packetHeader, packetSize;


			for( row = tgah->is_height - 1; row >= 0; row-- )
			{
				if( flipvert )
					ptr = &(*pixels)[ row * tgah->is_width * 4 ];

				for( col = 0; col < tgah->is_width; /* rien */ )
				{
					packetHeader	= *((unsigned char *)(pBuff++));
					packetSize		= 1 + (packetHeader & 0x7f);

					if( packetHeader & 0x80 )
					{
						// run-length packet
						switch( tgah->is_pixel_depth )
						{
							case 16:
							{
// RLE TRUE-COLOR BGR 16 BITS
								// read the current pixel
								unsigned short color = *((unsigned short *)pBuff);
								pBuff += sizeof( short );

								// convert bgr (16 bits) pixel into rgba (32 bits) pixel
								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
									ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
									ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
									ptr[3] = 255;
								}

								break;
							}

							case 24:
							{
// RLE TRUE-COLOR BGR 24 BITS
								// convert bgr (24 bits) pixel into rgba (32 bits) pixel
								RGBTRIPLEY *pix = (RGBTRIPLEY *)pBuff;
								pBuff += sizeof( RGBTRIPLEY );

								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = pix->rgbtRed;
									ptr[1] = pix->rgbtGreen;
									ptr[2] = pix->rgbtBlue;
									ptr[3] = 255;
								}

								break;
							}

							case 32:
							{
// RLE TRUE-COLOR BGR 32 BITS
								// convert bgr (32 bits) pixel into rgba (32 bits) pixel
								BGRAQUAD *pix = (BGRAQUAD *)pBuff;
								pBuff += sizeof( BGRAQUAD );

								for( i = 0; i < packetSize; i++, ptr += 4, col++ )
								{
									ptr[0] = pix->bgraRed;
									ptr[1] = pix->bgraGreen;
									ptr[2] = pix->bgraBlue;
									ptr[3] = pix->bgraAlpha;
								}

								break;
							}
						}
					}
					else
					{
						// non run-length packet

						for( i = 0; i < packetSize; i++, ptr += 4, col++ )
						{
							switch( tgah->is_pixel_depth )
							{
								case 16:
								{
// RLE TRUE-COLOR BGR 16 BITS
									// read the current pixel
									unsigned short color = *((unsigned short *)pBuff);
									pBuff += sizeof( short );

									// convert bgr (16 bits) pixel into rgba (32 bits) pixel
									ptr[0] = ((color & 0x7C00) >> 10) << 3;	// b->r
									ptr[1] = ((color & 0x03E0) >>  5) << 3;	// g->g
									ptr[2] = ((color & 0x001F) >>  0) << 3;	// r->b
									ptr[3] = 255;							// alpha

									break;
								}

								case 24:
								{
// RLE TRUE-COLOR BGR 24 BITS
									// convert bgr (24 bits) pixel into rgba (32 bits) pixel
									RGBTRIPLEY *pix = (RGBTRIPLEY *)pBuff;
									pBuff += sizeof( RGBTRIPLEY );

									ptr[0] = pix->rgbtRed;
									ptr[1] = pix->rgbtGreen;
									ptr[2] = pix->rgbtBlue;
									ptr[3] = 255;

									break;
								}

								case 32:
								{
// RLE TRUE-COLOR BGR 32 BITS
									// convert bgr (32 bits) pixel into rgba (32 bits) pixel
									BGRAQUAD *pix = (BGRAQUAD *)pBuff;
									pBuff += sizeof( BGRAQUAD );

									ptr[0] = pix->bgraRed;
									ptr[1] = pix->bgraGreen;
									ptr[2] = pix->bgraBlue;
									ptr[3] = pix->bgraAlpha;

									break;
								}
							}
						}
					}
				}
			}

			break;
		}

		default:
		{
			// unknown format
			//delete [] buffer;
			memoryFree(buffer);
			return 0;
		}
	}

	// free buffer memory
	//delete [] buffer;
	memoryFree(buffer);

	// return success
	return 1;
}

int loadLMP( const char *filename, unsigned char *pixels, int iLightmapsize )
{
	std::ifstream		file;			// file stream

	file.open( filename, std::ios::in | std::ios::binary );

	if( file.fail() )
		return 0;

	file.read( (char*)pixels, iLightmapsize * iLightmapsize * sizeof(unsigned int) );

	file.clear();
	file.close();
	
	return 1;
}
