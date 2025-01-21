#include <curl/curl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "raylib.h"

typedef struct screenSize
{
    u_int16_t Width;
    u_int16_t Heigth;
} screen;

int main(void)
{
    screen screen1 = {
        .Width = 1600,
        .Heigth = 900,
    };

	
    InitWindow(screen1.Width, screen1.Heigth, "SpotifyCheap");
    SetTargetFPS(60);

    InitAudioDevice();
    Music music = LoadMusicStream("Adele - Skyfall (Official Lyric Video).wav");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.3);

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

        BeginDrawing();

        ClearBackground(BLUE);

        int timeLength = GetMusicTimeLength(music);
        int timePlayed = GetMusicTimePlayed(music);
        char timeInfo[100];
        sprintf(timeInfo, "Time: %.2ds / %.2ds", timePlayed, timeLength);

        int textWidth = MeasureText(timeInfo, 20);
        DrawText(timeInfo, (screen1.Width - textWidth)/2, screen1.Heigth - 100, 20, DARKGRAY);

        EndDrawing();
    }

    return 0;
}
