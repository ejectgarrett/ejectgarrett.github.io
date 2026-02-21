#include <iostream>
#include <raylib.h>
#include <stdint.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    int dvd_speed_x = 5;
    int dvd_speed_y = 5;

    Texture mark = LoadTexture("mark.png");
    int dvd_width = mark.width;
    int dvd_height = mark.height;
    cout << "MarkTex Height:" << mark.height << endl;
    int dvd_x = dvd_width * 1.25;
    int dvd_y = dvd_height * 1.25;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
   
        dvd_x += dvd_speed_x;
        dvd_y += dvd_speed_y;

        if (dvd_x + dvd_width >= SCREEN_WIDTH || dvd_x <= 0)
        {
            dvd_speed_x *= -1;
        }

        if (dvd_y + dvd_height >= SCREEN_HEIGHT || dvd_y <= 0)
        {
            dvd_speed_y *= -1;
        }

        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(mark, dvd_x,dvd_y, WHITE);
        EndDrawing();
    }
    
    UnloadTexture(mark);
    CloseWindow();
}