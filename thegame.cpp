#include <iostream>
#include <raylib.h>
#include <stdint.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    int mark_speed_x = 5;
    int mark_speed_y = 5;

    Image mark = LoadImage("mark.png");
    ImageResize(&mark, mark.width * 0.5, mark.height * 0.5);
    int mark_width = mark.width;
    int mark_height = mark.height;
    cout << "MarkTex Height:" << mark.height << endl;
    int mark_x = mark_width;
    int mark_y = 1;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    Texture2D markTex = LoadTextureFromImage(mark);
    UnloadImage(mark);
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
   
        mark_x += mark_speed_x;
        mark_y += mark_speed_y;

        if (mark_x + mark_width >= SCREEN_WIDTH || mark_x <= 0)
        {
            mark_speed_x *= -1;
        }

        if (mark_y + mark_height >= SCREEN_HEIGHT || mark_y <= 0)
        {
            mark_speed_y *= -1;
        }

        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(markTex, mark_x, mark_y, WHITE);
        EndDrawing();
    }

    UnloadTexture(markTex);
    CloseWindow();
}