#include <iostream>
#include <raylib.h>
#include <stdint.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    cout << "HEIGHT: " << SCREEN_HEIGHT  << "WIDTH: " << SCREEN_WIDTH << endl;
    int targetFPS = 60;
    int ball_x = 100;
    int ball_y = 100;
    int ball_speed_x = 5;
    int ball_speed_y = 5;
    int ball_radius = 15;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);
    int color = 0;
    Color colors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
    while (!WindowShouldClose()){
   
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;

        if(ball_x + ball_radius >= SCREEN_WIDTH || ball_x - ball_radius <= 0)
        {
            cout << "X HIT!" << endl;
            ball_speed_x *= -1;
        }

        if(ball_y + ball_radius >= SCREEN_HEIGHT || ball_y - ball_radius <= 0)
        {
            cout << "Y HIT!" << endl;
            ball_speed_y *= -1;
        }
        
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_UP))
            color = (color + 1) % 6;
        if (IsKeyPressed(KEY_DOWN))
            color = (color - 1) % 6;

        DrawCircle(ball_x,ball_y,ball_radius, colors[color]);
        EndDrawing();
    }

    CloseWindow();
}