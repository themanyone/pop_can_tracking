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
#include <stdlib.h>
#include <math.h>
#include "pop_can_tracking.h"

unsigned int rowcol(unsigned int r, unsigned int c){
	return r*out_width*3+c*3;	
}

/* Compare a pixel with a given rgb value
 */
int cmpr(uint8_t *img,int *rgb){
	int score=abs(img[0]-rgb[0])+abs(img[1]-rgb[1])+abs(img[2]-rgb[2]);
	return score;
}

/* Make RGB from text aabbcc
 */
int *toRGB(char *a){//returns a malloc array of 3 ints. free it.
	char rr[3],gg[3],bb[3];
	int *rg=malloc(3 *sizeof(int));
	strncpy(rr,a,2);
	strncpy(gg,a+2,2);
	strncpy(bb,a+4,2);
	rg[0]=strtol(rr,0,16),
	rg[1]=strtol(gg,0,16),
	rg[2]=strtol(bb,0,16);
	return rg;
}

/* Draw a crosshairs at r,c
 */
void crosshairs(uint8_t *img,unsigned int r,unsigned int c,int *rgb){
	int i,j,row,col;
	//printf("r %i, c %i\n",r,c);
	if(!r&&!c)return;
	if(r<20)r=20;
	if(c<20)c=20;
	if(c>out_width-21)c=out_width-21;
	if(r>out_height-20)r=out_height-20;
	for(j=630;j-=5;){
		row=20*sin(j/100.0);
		col=20*cos(j/100.0);
		for(i=3;i--;){
			img[rowcol(row+r,col+c)+i]=rgb[i];//img[(row+r)*width*3+(col+c)*3+i]=rgb[i];
			img[rowcol(row+r,col+c+1)+i]=rgb[i];//img[(row+r)*width*3+(col+c+1)*3+i]=rgb[i];
			img[rowcol(row/2+r,col/2+c)+i]=0;//img[(row/2+r)*width*3+(col/2+c)*3+i]=0;
		}
	}
	for(row=r-20;row<r+20;row++)for(i=3;i--;)img[rowcol(row,c)+i]=255;//img[row*width*3+c*3+i]=255;
	for(col=c-20;col<c+20;col++)for(i=3;i--;)img[rowcol(r,col)+i]=255;//img[r*width*3+col*3+i]=255;
}

/* Add a blob to the array
 */
void blobadd(blob b,unsigned int r,unsigned int c){
	unsigned int i;
	for(i=0;i<99;i++){
		if(b[i].count==0){//belongs to a new blob
			b[i].r=r,b[i].c=c;
			b[i].count=1;
			return;//made a new blob, return
		}
		else{
			if(abs(r-b[i].r)<50&&abs(c-b[i].c)<50){//within range of blob
				b[i].r=( b[i].r * b[i].count + r ) / (b[i].count+1);
				b[i].c=( b[i].c * b[i].count + c ) / (b[i].count+1);
				b[i].count++;//count it!
				//printf("r %.2f, c %.2f, i %i, count %i\n",b[i].r,b[i].c,i,b[i].count);
				return;//found one, return
			}
		}
	}
}

