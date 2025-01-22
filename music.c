#include <curl/curl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "raylib.h"
#include "videodown.h"

void soundTime(Music music, struct screenSize screen)
{
    int timeLength = GetMusicTimeLength(music);
    int timePlayed = GetMusicTimePlayed(music);
    char timeInfo[100];
    sprintf(timeInfo, "Time: %.2d / %.2d", timePlayed, timeLength);

    int textWidth = MeasureText(timeInfo, 20);
    DrawText(timeInfo, (screen.Width - textWidth) / 2, screen.Heigth - 100, 20, DARKGRAY);
}

Rectangle textbox(struct screenSize screen, char* url, int16_t* letterCount, int16_t* framesCounter, bool* mouseOnText, bool* isFocused)
{

    Rectangle textBox = {screen.Width / 2.0f - 100, 180, 225, 50};

    if (CheckCollisionPointRec(GetMousePosition(), textBox))
    {
        *mouseOnText = true;
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

    SetTargetFPS(60);

    InitAudioDevice();
    Music music = LoadMusicStream("");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.3);

    //main loop
    while (!WindowShouldClose())
    {
        // update the music to stream
        UpdateMusicStream(music);

        // Checking the mouse collision
        Rectangle textBox = textbox(screen1, url, &letterCount, &framesCounter, &mouseOnText, &isFocused);

        //Pause and resume keys
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

        //background
        ClearBackground(RAYWHITE);

        //mouse box
        DrawRectangleRec(textBox, LIGHTGRAY);
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, isFocused ? RED : DARKGRAY);

        DrawText(url, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

        if (isFocused && ((framesCounter / 20) % 2 == 0))
        {
            DrawText("_", (int)textBox.x + 8 + MeasureText(url, 40), (int)textBox.y + 12, 40, MAROON);
        }

        //sound time
        soundTime(music, screen1);

        EndDrawing();
    }


    return 0;
}
