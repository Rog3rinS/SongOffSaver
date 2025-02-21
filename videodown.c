#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "videodown.h"


size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

char* idVideo(char* videoUrl)
{
    regex_t regex;
    regmatch_t matches[2];
    const char* pattern = "v=([a-zA-Z0-9_-]{11})";

    if (regcomp(&regex, pattern, REG_EXTENDED))
    {
        return NULL;
    }

    if (regexec(&regex, videoUrl, 2, matches, 0) == 0)
    {
        int idLength = matches[1].rm_eo - matches[1].rm_so;
        char* videoId = (char*)malloc(idLength + 1);
        if (!videoId)
        {
            return NULL;
        }
        strncpy(videoId, videoUrl + matches[1].rm_so, idLength);
        videoId[idLength] = '\0';
        regfree(&regex);
        return videoId;
    }

    regfree(&regex);
    return NULL;
}

void changeExtensionToPng(char* filename)
{
    char* dot = strrchr(filename, '.');
    if (dot != NULL)
    {
        strcpy(dot, ".png");
    }
}

void changeExtensionToWav(char* filename)
{
    char* dot = strrchr(filename, '.');
    if (dot != NULL)
    {
        strcpy(dot, ".wav");
    }
}

Video* videoCreate(char* videoUrl, VideoList* list)
{
  Video *New_Video = (Video *)malloc(sizeof(Video));
  if (!New_Video) {
    return NULL;
  }

  char *videoId = idVideo(videoUrl);
  if (!videoId) {
    free(New_Video);
    return NULL;
  }

  memset(New_Video, 0, sizeof(Video));
  if (list->head == NULL) {
    list->head = New_Video;
    list->tail = New_Video;
    snprintf(New_Video->videoUrl, sizeof(New_Video->videoUrl), "%s", videoUrl);
    snprintf(New_Video->thumbUrl, sizeof(New_Video->thumbUrl),
             "https://img.youtube.com/vi/%s/maxresdefault.jpg", videoId);
    New_Video->prev = NULL;
  } else {
    snprintf(New_Video->videoUrl, sizeof(New_Video->videoUrl), "%s", videoUrl);
    snprintf(New_Video->thumbUrl, sizeof(New_Video->thumbUrl),
             "https://img.youtube.com/vi/%s/maxresdefault.jpg", videoId);
    New_Video->next = NULL;
    list->tail->next = New_Video;
    list->tail = New_Video;
  }

    free(videoId);
    return New_Video;
}

int videoDownload(char* url, Video* video) {
    char command[MAX_URL_LEN];
    FILE* fp;

    snprintf(command, sizeof(command),
             "yt-dlp -x --audio-format wav -o \"data/%%(title)s.%%(ext)s\" "
             "--get-filename \"%s\"",
             url);
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command to get video filename\n");
        return 1;
    }

    if (fgets(video->videoFileName, sizeof(video->videoFileName), fp) != NULL) {
        video->videoFileName[strcspn(video->videoFileName, "\n")] = '\0'; // Remove newline
        changeExtensionToWav(video->videoFileName);
    } else {
        fprintf(stderr, "Failed to retrieve video filename\n");
        pclose(fp);
        return 1;
    }
    pclose(fp);

    snprintf(command, sizeof(command), "yt-dlp --write-thumbnail --convert-thumbnails png -o \"data/%%(title)s.%%(ext)s\" --print filename \"%s\"", url);
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command to get thumbnail filename\n");
        return 1;
    }

    if (fgets(video->thumbFileName, sizeof(video->thumbFileName), fp) != NULL) {
        video->thumbFileName[strcspn(video->thumbFileName, "\n")] = '\0'; // Remove newline
        changeExtensionToPng(video->thumbFileName);
    } else {
        fprintf(stderr, "Failed to retrieve thumbnail filename\n");
        pclose(fp);
        return 1;
    }
    pclose(fp);

    // Download the video and thumbnail
    snprintf(command, sizeof(command),
             "yt-dlp -x --audio-format wav -o \"data/%%(title)s.%%(ext)s\" "
             "--write-thumbnail --convert-thumbnails png \"%s\"",
             url);
    printf("Downloading video and thumbnail: %s\n", command);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Failed to download video or thumbnail\n");
        return 1;
    }

    printf("Video saved as: %s\n", video->videoFileName);
    printf("Thumbnail saved as: %s\n", video->thumbFileName);
    return 0;
}

VideoList* videoNewDownload(char* videoUrl, VideoList* list)
{
    Video *createdVideo = videoCreate(videoUrl, list);
    if (!createdVideo) {
      printf("Video creation failed!\n");
      free(list);
      return NULL;
    }

    if (videoDownload(videoUrl, createdVideo) != 0) {
      printf("Video download failed!\n");
      free(createdVideo);
      free(list);
      return NULL;
    }

    Video *temp = list->head;
    while (temp != NULL) {
      printf("name: %s\n", temp->videoFileName);
      temp = temp->next;
    }
  return list;
}

//lets say that each video is an object, what i need to do with all these objects 

