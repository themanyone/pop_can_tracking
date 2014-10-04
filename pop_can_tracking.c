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

#include "pop_can_tracking.h"

int main(int argc, char *argv[]) {
	uint8_t i;
	if(argc<4){
usage:	printf("Usage: %s movie.avi threshold rrggbb ...\n",argv[0]);
		return 1;
	}
	threshold=atoi(argv[2]);
	if(threshold<10){
		printf("err: threshold should be >= 10\n");goto usage;
	}
		
	for(i=0;argv[i+3];i++){
		rgb[i]=toRGB(argv[i+3]);
		//printf("rgb%i: %i %i %i\n",i,rgb[i][0],rgb[i][1],rgb[i][2]);
		blobs[i]=calloc(99,sizeof(struct blobstruct));//keep track of up to 99 blobs
	} rgbmax=i;
	
	av_register_all();
	if(!OpenVid(argv[1]))return -1;
	for(i=0;argv[i+2];i++){	free(blobs[i]);free(rgb[i]);}
	return 0;
}

