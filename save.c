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
#include "ffm.h"

void SaveVid(AVFrame *picture,const char *filename,int width, int height){
	static AVCodec *codec;
	static AVFrame *outpic;
    static AVCodecContext *c;
    static int i, j, out_size, outbuf_size;
    static FILE *f;
    static uint8_t *outbuf;
	struct SwsContext* encoderSwsContext;
	unsigned int row,column;
	if(!f){
		
		//printf("Video encoding %s\n",filename);

		/* find the mpegr video encoder */
		codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
		if (!codec) {
			fprintf(stderr, "codec not found\n");
			exit(1);
		}

		c= avcodec_alloc_context();
		//picture= avcodec_alloc_frame();

		/* put sample parameters */
		c->bit_rate = 2000000;
		/* resolution must be a multiple of two */
		c->width = width;
		c->height = height;
		/* frames per second */
		c->time_base= (AVRational){1,25};
		c->gop_size = 10; /* emit one intra frame every ten frames */
		c->max_b_frames=1;
		c->pix_fmt = PIX_FMT_YUV420P;

		/* open it */
		if (avcodec_open(c, codec) < 0) {
			fprintf(stderr, "could not open codec\n");
			exit(1);
		}

		f = fopen(filename, "wb");
		if (!f) {
			fprintf(stderr, "could not open %s\n", filename);
			exit(1);
		}

		/* alloc image and output buffer */
		outbuf_size=avpicture_get_size(PIX_FMT_YUV420P, width, height);
		outbuf=(uint8_t *)av_malloc(outbuf_size*sizeof(uint8_t));
		
		outpic=avcodec_alloc_frame();
		if(outpic==NULL)
			exit(-1);
		
		avpicture_fill((AVPicture *)outpic, outbuf, PIX_FMT_YUV420P,width, height);

	}
	if(picture){
		
		for (row = 20; row < out_height-20; row++) {
			for (column = 20; column < out_width-20; column++) {
				//approximate a color match ~50 or so should work
				for(i=0;i<rgbmax;i++){
 					if(cmpr(picture->data[0]+rowcol(row,column),rgb[i])<50) {
						//if(i==2)*picture->data[0]+rowcol(row,column)+1=255;
						blobadd(blobs[i],row,column);//add it to blobs
					}
				}
			}
		}
		//should make a sorted list of blobs by count
		for(i=0;i<rgbmax;i++){
			//~ most=0,best=0;
			for(j=0;j<99&&blobs[i][j].count;j++)
				//if(blobs[i][j].count>most)best=j,most=blobs[i][j].count;
				if(blobs[i][j].count>threshold)
					crosshairs(picture->data[0],blobs[i][j].r,blobs[i][j].c,rgb[i]);
			//if(i==2)printf("%i pixel object found!\n",most);//debug
/* 			if(most>1){
				crosshairs(picture->data[0],blobs[i][best].r,blobs[i][best].c,rgb[i]);
				printf("%.0f %.0f ",blobs[i][best].r,blobs[i][best].c);
			} */
			memset(blobs[i],0,99*sizeof(struct blobstruct));
		}printf("\n");
		//crosshairs(picture->data[0],100,200,rgb[0]);
		
		encoderSwsContext=sws_getContext(width,
				height,PIX_FMT_RGB24,width, height,
				PIX_FMT_YUV420P,SWS_BICUBIC|SWS_CPU_CAPS_MMX|SWS_CPU_CAPS_MMX2|
				SWS_CPU_CAPS_3DNOW,0,0,0);
		sws_scale(encoderSwsContext,(const uint8_t * const*)picture->data,picture->linesize,
				0,height,outpic->data,outpic->linesize);

        /* encode the image */
        out_size = avcodec_encode_video(c, outbuf, outbuf_size, outpic);
        //printf("encoding frame %3d (size=%5d)\n", i++, out_size);
        fwrite(outbuf, 1, out_size, f);
	}else{
		/* get the delayed frames */
  		for(; out_size; i++) {
			fflush(stdout);

			out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);
			//printf("write frame %3d (size=%5d)\n", i, out_size);
			fwrite(outbuf, 1, out_size, f);
		}

		/* add sequence end code to have a real mpeg file */
  		outbuf[0] = 0x00;
		outbuf[1] = 0x00;
		outbuf[2] = 0x01;
		outbuf[3] = 0xb7;
		fwrite(outbuf, 1, 4, f);
		fclose(f);
		av_free(outbuf);
		av_free(outpic);
		avcodec_close(c);
		av_free(c);
		printf("\n");
	}
}

int OpenVid(const char *fname){
	AVFormatContext *pFormatCtx;

	// Open video file
	if(avformat_open_input(&pFormatCtx, fname, NULL, NULL)!=0)
	  return -1; // Couldn't open file

	// Retrieve stream information
	if(av_find_stream_info(pFormatCtx)<0)
	  return -1; // Couldn't find stream information

	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, fname, 0);

	/* Now pFormatCtx->streams is just an array of pointers,
	of size pFormatCtx->nb_streams, so let's walk through it
	until we find a video stream. 
	 */
	int i,videoStream;
	AVCodecContext *pCodecCtx;

	// Find the first video stream
	videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	  if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
		videoStream=i;
		break;
	  }
	if(videoStream==-1)
	  return -1; // Didn't find a video stream

	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;

	/* The stream's information about the codec is in what we call
	the "codec context." This contains all the information
	about the codec that the stream is using, and now we
	have a pointer to it. But we still have to find the
	actual codec and open it:  */

	AVCodec *pCodec;

	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
	  fprintf(stderr, "Unsupported codec!\n");
	  return -1; // Codec not found
	}
	// Open codec
	if(avcodec_open(pCodecCtx, pCodec)<0)
	  return -1; // Could not open codec

	AVFrame *pFrame=avcodec_alloc_frame();

	AVFrame *pFrameRGB=avcodec_alloc_frame();
	if(pFrameRGB==NULL)
	  return -1;

	/* Even though we've allocated the frame, we still need a place
	to put the raw data when we convert it. We use avpicture_get_size
	to get the size we need, and allocate the space manually:  */

	uint8_t *buffer;
	int numBytes;

	out_width=pCodecCtx->width,out_height=pCodecCtx->height;

	// Determine required buffer size and allocate buffer
	numBytes=avpicture_get_size(PIX_FMT_RGB24, out_width, out_height);
	buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,out_width, out_height);
				  //  pCodecCtx->width, pCodecCtx->height);

	int frameFinished;
	AVPacket packet;

	i=0;
	while(av_read_frame(pFormatCtx, &packet)>=0) {
	  // Is this a packet from the video stream?
	  if(packet.stream_index==videoStream) {
		// Decode video frame
		avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
							 &packet);
		
		// Did we get a video frame?
		if(frameFinished) {
 			struct SwsContext* encoderSwsContext=sws_getContext(pCodecCtx->width,
					pCodecCtx->height,PIX_FMT_YUV420P,out_width, out_height,
					PIX_FMT_RGB24,SWS_BICUBIC|SWS_CPU_CAPS_MMX|SWS_CPU_CAPS_MMX2|
					SWS_CPU_CAPS_3DNOW|SWS_PRINT_INFO,0,0,0);
			sws_scale(encoderSwsContext,(const uint8_t * const*)pFrame->data,pFrame->linesize,
					0,pCodecCtx->height,pFrameRGB->data,pFrameRGB->linesize);

			SaveVid(pFrameRGB, "out.mpg", out_width, out_height);
		}
	  }
		
	  // Free the packet that was allocated by av_read_frame
	  av_free_packet(&packet);
	}
	SaveVid(NULL,NULL,0,0);

	// Free the RGB image
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);
	return 0;
}
