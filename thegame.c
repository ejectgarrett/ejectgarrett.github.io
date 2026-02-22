#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAXIMUM_CODELINES 15

typedef struct {Texture2D *a, *b; int x, y, health; float x_multiplier, y_multiplier;} Marks;
typedef struct {char *line;/* Pixel length */ int width, x, y; bool good;} CodeLine;

const char *lines[5] = {
    "Welcome my friend!", 
    "I am Mark Sheldon, lead character of Tufts CS!", 
    "This is the white room.", 
    "Here, you're in control.", 
    "No assignments. No autograders. No debugging. Just pure low-level programming bliss."
};

char *good_lines[MAXIMUM_CODELINES / 2] = {
    "int i = 5;",
    "if (5 < 7) { printf(\"splendid\"); }",
    "char *goobert = \"goobert\";",
    "int bread = 2;",
    "char f = 'c';",
    "int _string = 60;",
    "int gooberts_money = 5;"
};
char *bad_lines[MAXIMUM_CODELINES / 2] = {
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
void InitCodelines(CodeLine *codelines[]);
void UpdateCodeLines(Marks *marks, CodeLine *codelines[], float delta, Sound *good, Sound *bad);
void SpawnCodeLine(CodeLine *codelines);
void DrawCodeLine(Marks *marks, CodeLine *codeline, Sound *good, Sound *bad);
void DrawMark(Marks *marks, bool red);
void DrawDialogue(Font *font, int i, Marks *marks, float time_since_last);
void GameLoop(Marks *marks, Font *font);
void YouWin(Font *font);
void YouLose(Font *font);

Color DROP_SHADOW = (Color) {0, 0, 0, 100};


int SCREEN_WIDTH;
int SCREEN_HEIGHT;

#define MARK_SPEED 8
#define CODELINE_SPEED 7

int main() 
{
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Move Mark!");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();
    ToggleBorderlessWindowed();
    HideCursor();
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
            time_since_last = 0;
        }
        DrawDialogue(&comicsans, i, &marks, time_since_last);
        DrawTextEx(comicsans, enter_message, 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText(enter_message, 30) / 2), 
                SCREEN_HEIGHT - SCREEN_HEIGHT / 20}, 30, 0, BLACK);
        DrawFPS(10, 20);
        EndDrawing();
        if (i > 4)
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

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) marks->x_multiplier++;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) marks->x_multiplier--;
    if (IsKeyDown(KEY_UP)  || IsKeyDown(KEY_W)) marks->y_multiplier--;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) marks->y_multiplier++;

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
Color mark_color = RAYWHITE;
float mark_scale = 1;
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
    mark_scale = Lerp(mark_scale, 1, 0.1);
    mark_color = (Color){
        Lerp(mark_color.r, RAYWHITE.r, 0.1),
        Lerp(mark_color.g, RAYWHITE.g, 0.1),
        Lerp(mark_color.b, RAYWHITE.b, 0.1),
        Lerp(mark_color.a, RAYWHITE.a, 0.1)
    };

    // draw shadow
    DrawTexturePro(*curr_mark, source, shadow_destination, 
        (Vector2) {curr_mark->width / 2, curr_mark->height / 2}, mark_rotation, DROP_SHADOW);
    // draw mark
    DrawTexturePro(*curr_mark, source, destination, 
        (Vector2) {curr_mark->width / 2, curr_mark->height / 2}, mark_rotation, mark_color);
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
    CodeLine *codelines[MAXIMUM_CODELINES];
    for (int i = 0; i < MAXIMUM_CODELINES; i++) {
        codelines[i] = (CodeLine *)malloc(sizeof(CodeLine));
    }

    InitCodelines(codelines);
    Music music_track = LoadMusicStream("music.mp3");
    PlayMusicStream(music_track);
    Sound pickup_good = LoadSound("pickupgood.mp3");
    Sound pickup_bad = LoadSound("pickupbad.mp3");
    const char *game_message = "Defend Mark from the invalid syntaxes!\n         Don't let his health go NULL.";
    while (!WindowShouldClose())
    {
        delta = GetFrameTime() * 100;

        TakeMovementInput(marks, delta);
        
        UpdateMusicStream(music_track);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        UpdateCodeLines(marks, codelines, delta, &pickup_good, &pickup_bad);
        DrawMark(marks, true);

        if (marks->health < 0) {
            EndDrawing();
            YouLose(font);
            break;
        }

        if (GetTime() < 40) {
            DrawTextEx(*font, game_message, 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText(game_message, 30) / 2),
            SCREEN_HEIGHT / 20}, 30, 0, BLACK);
        }

        if (marks->health >= 0)
            DrawTextEx(*font, TextFormat("HEALTH = %d", marks->health), 
            (Vector2) {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 20}, 40, 0, BLACK);

        if (GetTime() >= 70) // Should be 70
        {
            EndDrawing();
            YouWin(font);
            break;
        }

        EndDrawing();
    }

    for (int i = 0; i < MAXIMUM_CODELINES; i++)
        free(codelines[i]);
    StopMusicStream(music_track);
    UnloadMusicStream(music_track);

}

void InitCodelines(CodeLine *codelines[])
{
    for (int i = 0; i < MAXIMUM_CODELINES / 2; i++) {
        srand(time(NULL) + i);
        
        codelines[i]->line = good_lines[i];
        codelines[i]->width = MeasureTextEx(GetFontDefault(), codelines[i]->line, 30, 0).x;
        codelines[i]->x = GetRandomValue(-5 * SCREEN_WIDTH, 0);
        codelines[i]->y = GetRandomValue(0, SCREEN_HEIGHT);
        //rand() * SCREEN_HEIGHT
        codelines[i]->good = true;
    }

    for (int i = MAXIMUM_CODELINES / 2; i < MAXIMUM_CODELINES - 1; i++) {
        srand(time(NULL) + i);

        codelines[i]->line = bad_lines[i - (MAXIMUM_CODELINES / 2)];
        codelines[i]->width = MeasureTextEx(GetFontDefault(), codelines[i]->line, 30, 0).x;
        codelines[i]->x = GetRandomValue(-5 * SCREEN_WIDTH, 0);
        codelines[i]->y = GetRandomValue(0, SCREEN_HEIGHT);
        codelines[i]->good = false;
    }
}

void UpdateCodeLines(Marks *marks, CodeLine *codelines[], float delta, Sound *good, Sound *bad)
{
    for (int i = 0; i < MAXIMUM_CODELINES; i++) {
        CodeLine *cur = codelines[i];
        cur->x += CODELINE_SPEED * delta;
        DrawCodeLine(marks, cur, good, bad);
    }
}

void DrawCodeLine(Marks *marks, CodeLine *codeline, Sound *good, Sound *bad)
{
    DrawRectangle(codeline->x - 6, codeline->y, codeline->width + 12, 30, BLACK);
    DrawTextEx(GetFontDefault(), codeline->line, 
        (Vector2) {codeline->x, codeline->y}, 30, 0, GRAY);

    BoundingBox codeline_bounds;
    // codeline_bounds.min = (Vector3){codeline->x - 6, codeline->y, -100};
    // codeline_bounds.max = (Vector3){codeline->x + codeline->width + 6, codeline->y + 30, 100};
    // bool collision = CheckCollisionBoxSphere(codeline_bounds, 
    //     (Vector3){marks->x, marks->y, 0}, marks->a)
    bool collision = CheckCollisionRecs(
        (Rectangle) {codeline->x - 6, codeline->y, codeline->width + 12, 30}, 
        (Rectangle) {marks->x - marks->b->width / 2, marks->y - marks->b->height / 2, marks->b->width, marks->b->height});
    if (collision && !codeline->good) {
        PlaySound(*bad);
        marks->health--;
        codeline->x = 0;
        codeline->y = SCREEN_WIDTH * -2;
        mark_color = RED;
    }
    else if (collision && codeline->good) {
        PlaySound(*good);
        marks->health++;
        codeline->x = 0;
        codeline->y = SCREEN_WIDTH * -2;
        mark_color = GREEN;
    }

    //CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius);
    //
    // CheckCollisionBoxSphere()
}

void YouWin(Font *font)
{
    Sound win = LoadSound("win.mp3");
    PlaySound(win);
    while (!WindowShouldClose() && GetCharPressed() == 0)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextEx(*font, "YOU WIN!", 
                (Vector2) {SCREEN_WIDTH / 2 - (MeasureText("YOU WIN!", 100) / 2),
                SCREEN_HEIGHT / 2}, 100, 0, GREEN);
        EndDrawing();
    }
    UnloadSound(win);
}

void YouLose(Font *font)
{
    Sound lose = LoadSound("lose.mp3");
    PlaySound(lose);
    while(!WindowShouldClose() && GetCharPressed() == 0) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextEx(*font, "YOU LOSE!", 
            (Vector2) {SCREEN_WIDTH / 2 - (MeasureText("YOU LOSE!", 100) / 2), 
            SCREEN_HEIGHT / 2}, 100, 0, RED);
        DrawTextEx(*font, "HEALTH = NULL", (Vector2) 
            {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 20}, 40, 0, RED);
        EndDrawing();
    }

    UnloadSound(lose);
}