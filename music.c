#include <curl/curl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "raylib.h"
#include "videodown.h"

//only works with one this far
void freeVideoList(VideoList *list) {
  if (!list) {
    return;
  }

  Video *current = list->head;
  while (current != NULL) {
    Video *next = current->next;
    free(current);
    current = next;
  }

  free(list);
}

void VideoDisplayer(VideoList *list) {
    if (!list || !list->head) return; 

    int HvideoBox = 0;
    int WvideoBox = 0;
    int col = 0;

    for (Video *vid = list->head; vid != NULL; vid = vid->next) {
        Rectangle videoBox = {WvideoBox, HvideoBox, 250, 200};
        DrawRectangleRec(videoBox, BLACK);
        WvideoBox += 260;
        if (++col == 4) {
            col = 0;
            WvideoBox = 0;
            HvideoBox += 210;
        }
    }
}

void soundTime(Music music, struct screenSize screen) {
  int timeLength = GetMusicTimeLength(music);
  int timePlayed = GetMusicTimePlayed(music);
  char timeInfo[100];
  sprintf(timeInfo, "Time: %.2d / %.2d", timePlayed, timeLength);

  int textWidth = MeasureText(timeInfo, 20);
  DrawText(timeInfo, (screen.Width - textWidth) / 2, screen.Heigth - 100, 20,
           DARKGRAY);
}

Rectangle createTextBox(struct screenSize screen, char* url, int16_t* letterCount, int16_t* framesCounter, bool* mouseOnText, bool* isFocused, int* cursorPos, VideoList* list)
{
  Rectangle textBox = {screen.Width / 2.0f - 250, screen.Heigth - 50, 500, 30};

  if (CheckCollisionPointRec(GetMousePosition(), textBox)) {
    *mouseOnText = true;
    SetMouseCursor(MOUSE_CURSOR_IBEAM);
  } else {
    *mouseOnText = false;
  }

    if (*mouseOnText && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *isFocused = true;
    }
    else if (!(*mouseOnText) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *isFocused = false;
    }

    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, *isFocused ? RED : DARKGRAY);

    if (*isFocused)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        const int Padding = 5;
        const int maxWidth = textBox.width - 10;
        int key = GetCharPressed();


        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (*letterCount < (int16_t)MAX_URL_LEN))
            {
                url[*cursorPos] = (char)key;
                url[*cursorPos + 1] = '\0';
                (*cursorPos)++;
                (*letterCount)++;
            }
            key = GetCharPressed();
        }

        const int backspaceDelay = 30;
        static int backspaceFrames = 0;

        if (IsKeyDown(KEY_BACKSPACE)) {
          if (backspaceFrames == 0 || backspaceFrames >= backspaceDelay) {
            if (*cursorPos > 0) {
              for (int i = *cursorPos - 1; i < *letterCount; i++) {
                url[i] = url[i + 1];
              }
              (*cursorPos)--;
              (*letterCount)--;
              url[*letterCount] = '\0';
            }
            backspaceFrames = (backspaceFrames == 0) ? 1 : backspaceFrames;
          } else {
            backspaceFrames++;
          }
        } else {
          backspaceFrames = 0;
        }

        if (IsKeyPressed(KEY_V) && IsKeyDown(KEY_LEFT_CONTROL))
        {
            const char* clipboardText = GetClipboardText();
            if (clipboardText != NULL)
            {
                strncpy(url, clipboardText, MAX_URL_LEN - 1);
                url[MAX_URL_LEN - 1] = '\0';
                *letterCount = strlen(url);
                *cursorPos = *letterCount;
            }
        }

        const int arrowdelay = 100;
        static int arrowframes = 0;

        const int arrowdelay1 = 100;
        static int arrowframes1 = 0;

        if (IsKeyDown(KEY_LEFT) && *cursorPos > 0) {
          if (arrowframes1 == 0 || arrowframes1 >= arrowdelay1) {
            (*cursorPos)--;
            arrowframes1 = 1;
          } else {
            arrowframes1++;
          }
        } else {
          arrowframes1 = 0;
        }

        if (IsKeyDown(KEY_RIGHT) && *cursorPos < *letterCount) {
          if (arrowframes == 0 || arrowframes >= arrowdelay) {
            (*cursorPos)++;
            arrowframes = 1;
          } else {
            arrowframes++;
          }
        } else {
          arrowframes = 0;
        }

        int startPos = 0;
        if (MeasureText(url, 20) > maxWidth) {
          while (MeasureText(url + startPos, 20) > maxWidth) {
            startPos++;
          }
        }

        int urlWidth = MeasureText(url, 20);
        if (urlWidth > maxWidth) {
            int offset = 0;
            while (MeasureText(url + offset, 20) > maxWidth) {
                offset++;
            }
            startPos = offset;
        }

        int cursorX = textBox.x + Padding + MeasureText(url + startPos, 20) -
                      MeasureText(url + *cursorPos, 20);

        if (cursorX < textBox.x + Padding) {
          while (cursorX < textBox.x + Padding && startPos > 0) {
            startPos--;
            cursorX = textBox.x + Padding + MeasureText(url + startPos, 20) -
                      MeasureText(url + *cursorPos, 20);
          }
        }

        //THIS FUCKING PIECE OF SHIT DOESNT WORK WHEN THE CURSOR GOES TO THE LEFT
        DrawText(url + startPos, textBox.x + Padding, textBox.y + Padding, 20, BLACK);

        if (((*framesCounter / 30) % 2) == 0) {
          if (cursorX >= textBox.x + Padding &&
              cursorX < textBox.x + textBox.width - 10) {
            DrawText("|", cursorX, (int)textBox.y + 5, 20, MAROON);
          }
        }

        if (IsKeyDown(KEY_ENTER))
        {
          videoNewDownload(url, list);
        }
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        const int Padding = 5;
        const int maxWidth = textBox.width - 10;
        int urlWidth = MeasureText(url, 20);
        int startPos = 0;

        if (urlWidth > maxWidth) {
            int offset = 0;
            while (MeasureText(url + offset, 20) > maxWidth) {
                offset++;
            }
            startPos = offset;
        }

        DrawText(url + startPos, textBox.x + Padding, textBox.y + Padding, 20, BLACK);
    }

    if (*isFocused)
    {
        (*framesCounter)++;
    }
    else
    {
        (*framesCounter) = 0;
    }

    return textBox;
}

int main(void)
{
    screen screen1 = {
        .Width = 1600,
        .Heigth = 900,
    };
    InitWindow(screen1.Width, screen1.Heigth, "SpotifyCheap");

    char url[MAX_URL_LEN + 1] = "\0";
    int16_t letterCount = 0;
    int16_t framesCounter = 0;
    bool mouseOnText = false;
    bool isFocused = false;
    printf("someshitisgoingon");
    int cursorPos = 0;
    VideoList *list = (VideoList *)malloc(sizeof(VideoList));
    if (!list) {
      printf("memmory allocation failed\n");
      exit(1);
    }

    list->head = NULL;
    list->tail = NULL;

    SetTargetFPS(60);

    InitAudioDevice();
    Music music = LoadMusicStream("");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.3);

    //main loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE))
        {
            if (IsMusicStreamPlaying(music))
            {
                PauseMusicStream(music);
            }
            else
            {
                ResumeMusicStream(music);
            }
        }


        //Draw -------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        createTextBox(screen1, url, &letterCount, &framesCounter, &mouseOnText, &isFocused, &cursorPos, list);
        //need to create the list properly and not set head and tail to null, if i do this shit will never work
        if (list) {
          VideoDisplayer(list);
        }

        soundTime(music, screen1);

        EndDrawing();
    }

    return 0;
}
