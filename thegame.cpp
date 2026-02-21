#include <iostream>
#include <raylib.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = GetMonitorWidth(0);
    const int SCREEN_HEIGHT = GetMonitorHeight(0);
    ToggleFullscreen();
    int targetFPS = GetMonitorRefreshRate(0);
    int ball_x = 100;
    int ball_y = 100;
    int ball_speed_x = 5;
    int ball_speed_y = 5;
    int ball_radius = 15;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(165);
    bool color = false;
    while (WindowShouldClose() == false){
   
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;

        if(ball_x + ball_radius >= SCREEN_WIDTH || ball_x - ball_radius <= 0)
        {
            ball_speed_x *= -1;
        }

        if(ball_y + ball_radius >= SCREEN_HEIGHT || ball_y - ball_radius <= 0)
        {
            ball_speed_y *= -1;
        }
        
        BeginDrawing();
            ClearBackground(BLACK);
        if (IsKeyPressed(KEY_H))
            color = !color;
        if (color)
            DrawCircle(ball_x,ball_y,ball_radius, BLUE);
        else
            DrawCircle(ball_x,ball_y,ball_radius, WHITE);
        EndDrawing();
    }

    CloseWindow();
}