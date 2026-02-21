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
    SetTargetFPS(GetMonitorRefreshRate(0));
    while (!WindowShouldClose()) {
   
        if (IsKeyDown(KEY_RIGHT) && mark_x < SCREEN_WIDTH - mark_width)
            mark_x += mark_speed_x;

        if (IsKeyDown(KEY_LEFT) && mark_x > mark_speed_x)
            mark_x -= mark_speed_x;

        if (IsKeyDown(KEY_UP) && mark_y > mark_speed_y)
            mark_y -= mark_speed_y;

        if (IsKeyDown(KEY_DOWN) && mark_y < SCREEN_HEIGHT - mark_height)
            mark_y += mark_speed_y;
        
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(markTex, mark_x, mark_y, WHITE);
        EndDrawing();
    }

    UnloadTexture(markTex);
    CloseWindow();
}