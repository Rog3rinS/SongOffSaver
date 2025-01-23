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
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            (*letterCount)--;
            if (*letterCount < 0)
                *letterCount = 0;
            url[*letterCount] = '\0';
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
        if (*letterCount > 46)
        {
            int maxVisibleChars = 46;

            int startPos = *letterCount - maxVisibleChars;
            if (startPos < 0)
                startPos = 0;

            int visibleChars = *letterCount - startPos;
            if (visibleChars > maxVisibleChars)
                visibleChars = maxVisibleChars;

            char tempBuffer[maxVisibleChars + 1];
            strncpy(tempBuffer, url + startPos, visibleChars);
            tempBuffer[visibleChars] = '\0';

            DrawText(tempBuffer, textBox.x + 5, textBox.y + 5, 20, BLACK);
        }
        else
        {
            DrawText(url, textBox.x + 5, textBox.y + 5, 20, BLACK);
        }
    }
    else
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
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
