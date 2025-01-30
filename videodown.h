#ifndef VIDEO_DOWNLOAD_H
#define VIDEO_DOWNLOAD_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define MAX_URL_LEN 1024

typedef struct screenSize
{
    u_int16_t Width;
    u_int16_t Heigth;
} screen;

typedef struct Video
{
    char videoUrl[256];
    char videoFileName[128];
    char thumbUrl[256];
    char thumbFileName[128];
    struct Video* next;
    struct Video* prev;
} Video;

typedef struct VideoList {
  Video *head;
  Video *tail;
} VideoList;

Video* videoNewDownload(char* videoUrl);
//    Rectangle createVideoBox();

#endif
