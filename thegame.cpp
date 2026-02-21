#include <iostream>
#include <raylib.h>
#include <stdint.h>

using namespace std;

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    cout << "HEIGHT: " << SCREEN_HEIGHT  << "WIDTH: " << SCREEN_WIDTH << endl;
    int targetFPS = 60;
    int dvd_x = 200;
    int dvd_y = 100;
    int dvd_speed_x = 5;
    int dvd_speed_y = 5;
    int dvd_width = 184;
    int dvd_height = 76;
    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);
    int color = 0;
    Color colors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
    Texture2D dvd = LoadTexture("dvd.png");
    Camera2D camera;
    camera.offset.x = SCREEN_WIDTH / 2;
    camera.offset.y = SCREEN_HEIGHT / 2;
    camera.target.x = 0;
    camera.target.y = 0;
    camera.zoom = 1;

    Vector2 player_position;
    player_position.x = 0;
    player_position.y = 0;

    while (!WindowShouldClose()) {
        // upd camera
        camera.target.x = player_position.x;
        camera.target.y = player_position.y;
   
        BeginDrawing();

        BeginMode2D(camera);
        DrawTexture(dvd, 0, 0, WHITE);
        EndMode2D();

        ClearBackground(BLACK);
        EndDrawing();
    }
    UnloadTexture(dvd);
    CloseWindow();
}