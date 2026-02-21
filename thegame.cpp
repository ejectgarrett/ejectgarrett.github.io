#include <iostream>
#include <raylib.h>
#include <stdint.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    cout << "HEIGHT: " << SCREEN_HEIGHT  << "WIDTH: " << SCREEN_WIDTH << endl;
    int targetFPS = 60;
    int dvd_x = 100;
    int dvd_y = 100;
    int dvd_speed_x = 5;
    int dvd_speed_y = 5;
    int dvd_radius = 200;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);
    int color = 0;
    Color colors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
    Texture2D dvd = LoadTexture("dvd.png");
    while (!WindowShouldClose()) {
   
        dvd_x += dvd_speed_x;
        dvd_y += dvd_speed_y;

        if (dvd_x + dvd_radius >= SCREEN_WIDTH || dvd_x - dvd_radius <= 0)
        {
            dvd_speed_x *= -1;
        }

        if (dvd_y + dvd_radius >= SCREEN_HEIGHT || dvd_y - dvd_radius <= 0)
        {
            dvd_speed_y *= -1;
        }

        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(dvd, dvd_x,dvd_y, WHITE);
        EndDrawing();
    }
    UnloadTexture(dvd);
    CloseWindow();
}