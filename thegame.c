#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

typedef struct {Texture2D *a, *b; int x, y, health; float x_multiplier, y_multiplier;} Marks;
typedef struct {char **line; int size, x, y; bool good;} CodeLine;

const char *lines[5] = {
    "Welcome my friend!", 
    "I am Mark Sheldon, lead character of Tufts CS!", 
    "This is the white room.", 
    "Here, you're in control.", 
    "No assignments. No autograders. No debugging. Just pure low-level programming bliss."
};

const char *good_lines[7] = {
    "int i = 5;",
    "if (5 < 7) { printf(\"splendid\"); }",
    "char *goobert = \"goobert\";",
    "int bread = 2;",
    "char f = 'c';",
    "int _string = 60;",
    "int gooberts_money = 5;"
};
const char *bad_lines[7] = {
    "int string = 5;",
    "if (true) printf(\"doogle\") }",
    "string bob = 5.5;",
    "char c = \"d\";",
    "char *int = \"bobert\";",
    "char *name = \"barry;",
    "float 5 = 5.0;",
};

void TakeMovementInput(Marks *marks, float delta);
float Clamp(float v, float _min, float _max);
float Lerp(float a, float b, float interp);
void UpdateCodeLines(CodeLine *codelines, float delta);
void SpawnCodeLine(CodeLine *codelines);
void DrawCodeLine(Marks *marks, CodeLine *codeline);
void DrawMark(Marks *marks, bool red);
void DrawDialogue(Font *font, int i, Marks *marks, float time_since_last);
void GameLoop(Marks *marks, Font *font);
void YouWin(Font *font);
void YouLose(Font *font);

Color DROP_SHADOW = (Color) {0, 0, 0, 100};

int SCREEN_WIDTH = 1;
int SCREEN_HEIGHT = 1;

int MARK_SPEED = 8;
const int MAXIMUM_CODELINES = 15;

int main() 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Move Mark!");
    ToggleBorderlessWindowed();
    HideCursor();
    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();
    Font comicsans = LoadFont("comicsans.ttf");
    Image mark = LoadImage("mark.png");
    Image redmark = LoadImage("redmark.png");

    int scalar = SCREEN_WIDTH / 10;
    ImageResize(&mark, SCREEN_WIDTH / 10, SCREEN_WIDTH / 10 * ((float)mark.height / (float)mark.width));
    ImageResize(&redmark, SCREEN_WIDTH / 10, SCREEN_WIDTH / 10 * ((float)mark.height / (float)mark.width));
    int mark_width = mark.width;
    int mark_height = mark.height;
    int mark_x = SCREEN_WIDTH / 2;
    int mark_y = SCREEN_HEIGHT / 2;
    Texture2D markTex = LoadTextureFromImage(mark);
    Texture2D redmarkTex = LoadTextureFromImage(redmark);

    Marks marks;
    marks.a = &markTex;
    marks.b = &redmarkTex;
    marks.x = mark_x;
    marks.y = mark_y;
    marks.health = 0;
    marks.x_multiplier = 0;
    marks.y_multiplier = 0;

    UnloadImage(mark);
    UnloadImage(redmark);
    SetTargetFPS(GetMonitorRefreshRate(0));
    int i = 0;

    Vector2 text_origin = {0.5, 0};
    float delta = 0.0;
    float text_width = 1;
    
    float time_since_last = 0;
    const char *enter_message = "Press Enter to progress dialogue";
    
        InitAudioDevice();
        Music music_track = LoadMusicStream("calm.mp3");
        PlayMusicStream(music_track);
    
    while (!WindowShouldClose()) 
    {
        delta = GetFrameTime() * 100;
        UpdateMusicStream(music_track);
        TakeMovementInput(&marks, delta);
        
        time_since_last += delta;

        // ====================================================================

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMark(&marks, false);
        if (IsKeyPressed(KEY_ENTER)) {
            i++;
            i %= 5;
            time_since_last = 0;
        }
        DrawDialogue(&comicsans, i, &marks, time_since_last);
        DrawTextEx(comicsans, enter_message, 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText(enter_message, 30) / 2), 
                SCREEN_HEIGHT - SCREEN_HEIGHT / 20}, 30, 0, BLACK);
        DrawFPS(10, 20);
        EndDrawing();
        if (GetTime() >= 10)
        {
            StopMusicStream(music_track);
            UnloadMusicStream(music_track);
            GameLoop(&marks, &comicsans);
            break;
        }
    }

    UnloadTexture(markTex);
    CloseWindow();
    UnloadFont(comicsans);
    ShowCursor();
}

float Clamp(float v, float _min, float _max) 
{
    return MAX(MIN(_max, v), _min);
}

float Lerp(float a, float b, float interp) 
{
    return a + interp * (b - a);
}

void TakeMovementInput(Marks *marks, float delta) 
{
    marks->x_multiplier = 0;
    marks->y_multiplier = 0;

    if (IsKeyDown(KEY_RIGHT)) marks->x_multiplier++;
    if (IsKeyDown(KEY_LEFT)) marks->x_multiplier--;
    if (IsKeyDown(KEY_UP)) marks->y_multiplier--;
    if (IsKeyDown(KEY_DOWN)) marks->y_multiplier++;

    // normalize vector
    float magnitude = 
        sqrt(marks->x_multiplier * marks->x_multiplier + 
            marks->y_multiplier * marks->y_multiplier);
    if (magnitude != 0) {
        marks->x_multiplier /= magnitude;
        marks->y_multiplier /= magnitude;
    }

    // move mark
    marks->x += marks->x_multiplier * MARK_SPEED * delta;
    marks->y += marks->y_multiplier * MARK_SPEED * delta;
    // clamp mark
    marks->x = Clamp(marks->x, 0, SCREEN_WIDTH);
    marks->y = Clamp(marks->y, 0, SCREEN_HEIGHT);
}

float mark_rotation = 0;
void DrawMark(Marks *marks, bool red) 
{
    Texture2D *curr_mark; 
    if (red)
        curr_mark = marks->b;
    else
        curr_mark = marks->a;
    
    Rectangle source = {0, 0, marks->a->width, marks->a->height};
    Rectangle destination = {marks->x, marks->y, marks->a->width, curr_mark->height};
    Rectangle shadow_destination = {marks->x + 15, marks->y + 15, marks->a->width, 
        curr_mark->height};

    mark_rotation = Lerp(mark_rotation, marks->x_multiplier * 5, 0.12f);

    // draw shadow
    DrawTexturePro(*curr_mark, source, shadow_destination, 
        (Vector2) {curr_mark->width / 2, curr_mark->height / 2}, mark_rotation, DROP_SHADOW);
    // draw mark
    DrawTexturePro(*curr_mark, source, destination, 
        (Vector2) {curr_mark->width / 2, curr_mark->height / 2}, mark_rotation, RAYWHITE);
}
//Marks *marks
//Font *font, int i, int mark_x, int mark_y, int mark_width, int mark_height, float time_since_last
void DrawDialogue(Font *font, int i, Marks *marks, float time_since_last)
{
    time_since_last *= 0.1;

    float animated_font_size = -(time_since_last - 0.75) * (time_since_last - 0.75) + 1.15;
    animated_font_size = Clamp(animated_font_size, 1, 2);
    animated_font_size *= 30;
    // float animated_rotation = Clamp(1 - 0.5 * time_since_last, 0, 100) * 
    //     cos(7 * time_since_last);
    float animated_y = Clamp(1 - 0.5 * time_since_last, 0, 100) * 
        cos(4 * time_since_last);
    animated_y = abs(animated_y);

    //MeasureTextEx(Font font, const char *text, float fontSize, float spacing)
    float text_width = MeasureTextEx(*font, lines[i], animated_font_size, 0).x;

    DrawTextPro(*font, lines[i], 
        (Vector2){marks->x, 
            marks->y - marks->a->height / 2 - SCREEN_HEIGHT / 30 + animated_y * 30}, 
        (Vector2){text_width / 2, 0}, 0, animated_font_size, 1, BLACK);
}


void GameLoop(Marks *marks, Font *font)
{
    float delta = 0.0;
    CodeLine codelines[MAXIMUM_CODELINES];
    Music music_track = LoadMusicStream("music.mp3");
    PlayMusicStream(music_track);
    const char *game_message = "Defend Mark from the invalid syntaxes!\n         Don't let his health go NULL.";
    while (!WindowShouldClose())
    {
        delta = GetFrameTime() * 100;

        TakeMovementInput(marks, delta);
        
        UpdateMusicStream(music_track);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawMark(marks, true);

        if (marks->health <0 -1)
        {
            EndDrawing();
            YouLose(font);
            break;
        }

        if (GetTime() < 40)
        {
            DrawTextEx(*font, game_message, 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText(game_message, 30) / 2), 
            SCREEN_HEIGHT / 20}, 30, 0, BLACK);
        }

        if (marks->health >= 0)
            DrawTextEx(*font, TextFormat("HEALTH = %d", marks->health), (Vector2) {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 20}, 40, 0, BLACK);
            

        if (GetTime() >= 70)
        {
            EndDrawing();
            YouWin(font);
            break;
        }

        EndDrawing();
    }
    StopMusicStream(music_track);
    UnloadMusicStream(music_track);

}
int CODELINE_SPEED = 7;
void UpdateCodeLines(CodeLine *codelines, float delta)
{
    for (int i = 0; i < MAXIMUM_CODELINES; i++) {
        CodeLine *cur = codelines + i;
        cur->x += CODELINE_SPEED * delta;
        DrawCodeLine(NULL, cur);
    }
}

void DrawCodeLine(Marks *marks, CodeLine *codeline)
{
    float text_width = 
        MeasureTextEx(GetFontDefault(), codeline->line, , 0).x;
}

void YouWin(Font *font)
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextEx(*font, "YOU WIN!", 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText("YOU WIN!", 100) / 2), 
            SCREEN_HEIGHT / 2}, 100, 0, GREEN);
        EndDrawing();
    }
}

void YouLose(Font *font)
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextEx(*font, "YOU LOSE!", 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText("YOU LOSE!", 100) / 2), 
            SCREEN_HEIGHT / 2}, 100, 0, RED);
        DrawTextEx(*font, "HEALTH = NULL", (Vector2) {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 20}, 40, 0, RED);
        EndDrawing();
    }
}