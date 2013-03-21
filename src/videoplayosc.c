/* $Id: videoplayosc.c 54 2009-02-06 14:54:20Z nescivi $ 
 *
 * Copyright (C) 2013, Marije Baalman <nescivi _at_ gmail.com>
 *
 * based on examples for
 * motempl.c from samples/c from the openCV library
 * and liblo by Steve Harris, Uwe Koloska
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _EiC
// motion templates sample code
#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <lo/lo.h>

#endif

/* struct{ */
/*   var x; */
/*   var y; */
/* } motionpoint; */


int done = 0;
int paused = 0;
int recording = 0;
int current_frame = 0;

lo_address t;
lo_server s;
lo_server_thread st;

char frametimestring [255];

    int line_type = CV_AA; // change it to 8 to see non-antialiased graphics
    CvFont font;
    CvSize text_size;
    CvPoint text_pos;

//CvPoint *track_points;

void error(int num, const char *m, const char *path);
int frame_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);
int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);
int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data);


// char filename[255];
char *filename;
// int filecounter = 0;
double fps;

//CvVideoWriter *writer;
CvSize camSize;

CvCapture* capture = 0;


/*
void createNewPlayer(){
	sprintf( filename, "%s_%i.avi", basefilename, filecounter );
	filecounter++;
	frame = 0;
	writer = cvCreateVideoWriter(
		filename,
	 	CV_FOURCC('M','J','P','G'),
	 	fps,
	 	camSize,
		1
	);
}

int set_recording( int rec )
{
	recording = rec;
	if ( recording ){
		if ( writer ){
			lo_send_from( t, s, LO_TT_IMMEDIATE, "/videorec/record/stop", "s", filename );
			cvReleaseVideoWriter( &writer );
		}
		createNewWriter();
		lo_send_from( t, s, LO_TT_IMMEDIATE, "/videorec/record/start", "s", filename );
	} else {
		lo_send_from( t, s, LO_TT_IMMEDIATE, "/videorec/record/stop", "s", filename );
		cvReleaseVideoWriter( &writer );
	}
   return recording;
}
*/

int showFrame( int frameid ){
  if ( current_frame == frameid ){
      return 0;
  }
  if ( (current_frame + 1) != frameid ){ // have to skip
    cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, frameid );
  }
  IplImage* image;
  image = cvQueryFrame( capture );
  if( image ){
//       sprintf( frametimestring, "frame: %i", frame);
//       cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX, 1, 1, 0.0, 1, line_type );
//       text_pos = cvPoint( 10, image->height-80 );
//       cvPutText( image, filename, text_pos, &font, CV_RGB(255,255,0));
//       text_pos = cvPoint( 10, image->height-50 );
//       cvPutText( image, frametimestring, text_pos, &font, CV_RGB(255,255,0));
    cvShowImage( "CameraImage", image );
    current_frame = frameid;
    return 0;
  }
  return -1;
}

int main(int argc, char** argv)
{

    char *port = "57151";
    char *outport = "57120";
    char *ip = "127.0.0.1";

    char *input = "0";
//     filename = "test";

    IplImage* motion = 0;

    printf("argv: %s %s %s %s %s %s %i\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argc );

    filename = argv[1];
    
    capture = cvCaptureFromFile( filename );
    
    if ( argc == 5 )
	    {
	    ip = argv[4];
	    port = argv[3];
	    outport = argv[2];
	    }
    else if ( argc == 4 )
	    {
	    port = argv[3];
	    outport = argv[2];
	    }
    else if ( argc == 3 )
	    {
	    outport = argv[2];
	    }

    printf("============================================================================\n");
    printf("videoplayosc - v0.1 - sends out osc data based on video recording\n");
    printf("                     (c) 2013, Marije Baalman\n");
    printf("                http://www.nescivi.nl/videorecosc\n");
    printf("Written using OpenVC and liblo\n");
    printf("This is free software released under the GNU/General Public License\n");
    printf("start with \"videoplayosc <file> <target_port> <recv_port> <target_ip>\" \n");
    printf("============================================================================\n\n");
    printf("video input from: %s\n", filename );
    printf("Listening to port: %s\n", port );
    printf("Sending to ip and port: %s %s\n", ip, outport );
    fflush(stdout);

//	print_help();

    /* create liblo addres */
    t = lo_address_new(ip, outport); // change later to use other host

    lo_server_thread st = lo_server_thread_new(port, error);

    lo_server_thread_add_method(st, "/videoplay/frame", "i", frame_handler, NULL);

    lo_server_thread_add_method(st, "/videoplay/quit", "", quit_handler, NULL);
    lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);

    lo_server_thread_start(st);
 
    lo_server s = lo_server_thread_get_server( st );

    lo_send_from( t, s, LO_TT_IMMEDIATE, "/videoplay/started", "" );

    if( capture )
    {

//	printf( "capturing at framerate %f\n", cvGetCaptureProperty( capture, CV_CAP_PROP_FPS ) );
	fps = cvGetCaptureProperty (
	 	capture,
	 	CV_CAP_PROP_FPS
	);
	printf( "capturing at framerate %f\n", fps );
	if ( fps == -1 ){ fps = 25; }
	lo_send_from( t, s, LO_TT_IMMEDIATE, "/videoplay/fps", "f", fps );

	cvNamedWindow( "VideoImage", 1 );

	// get first image:
	IplImage* image1;
	image1 = cvQueryFrame( capture );
	if( image1 ){
		camSize = cvSize( image1->width, image1->height );
	}
        
	while( !done ){
	      int c = cvWaitKey(40);
	      switch( (char) c ){
		      case '\x1b':
			      printf("Exiting ...\n");
			      goto exit_main;
		      }
	}
	exit_main:
	  cvReleaseCapture( &capture );
// 	  cvReleaseVideoWriter( &writer );
	  cvDestroyWindow( "CameraImage" );
   }

   lo_send_from( t, s, LO_TT_IMMEDIATE, "/videoplay/quit", "s", "nothing more to do, quitting" );
   lo_server_thread_free( st );
   lo_address_free( t );

   return 0;
}

void error(int num, const char *msg, const char *path)
{
     printf("liblo server error %d in path %s: %s\n", num, path, msg);
     fflush(stdout);
}


int frame_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
   showFrame( argv[0]->i );
   return 0;
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data)
{
    int i;

    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++) {
	printf("arg %d '%c' ", i, types[i]);
	lo_arg_pp(types[i], argv[i]);
	printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
		 void *data, void *user_data)
{
    done = 1;
    printf("videoplayosc: allright, that's it, I quit\n");
    fflush(stdout);

    return 0;
}

                                
#ifdef _EiC
main(1,"videoplayosc2.c");
#endif
