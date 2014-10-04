/* Pop Can Tracking
 * Copyright (C) 2011, 2014 Henry Kroll, www.thenerdshow.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef _FFM_H
#define _FFM_H

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

typedef struct blobstruct {
	float r;
	float c;
	unsigned int count;	
} *blob;
blob blobs[99];//99 rgb pointers to blob struct

int *rgb[99],rgbmax;

unsigned int out_width,out_height,threshold;

void crosshairs(uint8_t *img,unsigned int r,unsigned int c,int *rgb);
unsigned int rowcol(unsigned int r, unsigned int c);
void blobadd(blob b,unsigned int r,unsigned int c);
int cmpr(uint8_t *img,int *rgb);
int OpenVid(const char *fname);
int *toRGB(char *a);

#endif