#ifndef VIDEO_DOWNLOAD_H
#define VIDEO_DOWNLOAD_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
} Video;

size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream);
void changeExtensionToWebp(char* filename);
char* idVideo(char* videoUrl);
Video videoCreate(char* videoUrl);
int videoDownload(char* url, Video* video);

#endif
