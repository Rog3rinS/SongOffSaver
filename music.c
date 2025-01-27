#include <curl/curl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "raylib.h"
#include "videodown.h"

//need to add key bind, like c-v and c-c, and so that the text doesnt get out of the box

void soundTime(Music music, struct screenSize screen)
{
    int timeLength = GetMusicTimeLength(music);
    int timePlayed = GetMusicTimePlayed(music);
    char timeInfo[100];
    sprintf(timeInfo, "Time: %.2d / %.2d", timePlayed, timeLength);

    int textWidth = MeasureText(timeInfo, 20);
    DrawText(timeInfo, (screen.Width - textWidth) / 2, screen.Heigth - 100, 20, DARKGRAY);
}

Rectangle createTextBox(struct screenSize screen, char* url, int16_t* letterCount, int16_t* framesCounter, bool* mouseOnText, bool* isFocused)
{
    //drawing, and textBox
    Rectangle textBox = {screen.Width / 2.0f - 250, screen.Heigth / 2.0f, 500, 30};

    if (CheckCollisionPointRec(GetMousePosition(), textBox))
    {
        *mouseOnText = true;
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }
    else
    {
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
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, isFocused ? RED : DARKGRAY);

    int startPos = 0;
    // doing mouse stuff
    if (*isFocused)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (*letterCount < (int16_t)MAX_URL_LEN))
            {
                url[*letterCount] = (char)key;  // casting just in case
                url[*letterCount + 1] = '\0';   // no segfault here buddy
                (*letterCount)++;
            }
            key = GetCharPressed();
        }

        const int backspaceDelay = 30;
        static int backspaceFrames = 0;

        if (IsKeyDown(KEY_BACKSPACE))
        {
            if (backspaceFrames == 0 || backspaceFrames >= backspaceDelay)
            {
              if (*letterCount > 0)
              {
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

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
        {
            const char* clipboardText = GetClipboardText();
            if (clipboardText != NULL)
            {
                strncpy(url, clipboardText, MAX_URL_LEN - 1);
                url[MAX_URL_LEN - 1] = '\0';
                *letterCount = strlen(url);
            }
        }

        //not good, but it works (actually its very shity)
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

        if (*letterCount < MAX_URL_LEN) {
          if (((*framesCounter / 20) % 2) == 0) {
                int visibleTextWidth = MeasureText(url, 20);
                int cursorX = textBox.x + 5 + visibleTextWidth;
                if (cursorX < textBox.x + textBox.width - 10) {
                  DrawText("|", cursorX, (int)textBox.y + 5, 20, MAROON);
                }

          }
        }

        if (IsKeyDown(KEY_ENTER))
        {
          videoNewDownload(url);
        }
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        const int maxVisibleChars = 45;
        startPos = (*letterCount > maxVisibleChars)
                       ? *letterCount - maxVisibleChars
                       : 0;
        DrawText(url + startPos, textBox.x + 5, textBox.y + 5, 20, BLACK);
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

        createTextBox(screen1, url, &letterCount, &framesCounter, &mouseOnText, &isFocused);

        soundTime(music, screen1);

        EndDrawing();
    }

    return 0;
}
