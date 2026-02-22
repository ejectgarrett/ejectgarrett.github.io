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
#define MAXIMUM_CODELINES 50
#define TILE_SIZE SCREEN_WIDTH / 16
#define CINEMATIC_BAR_SIZE SCREEN_HEIGHT / 20
#define SMALL_FONT SCREEN_WIDTH / 85
#define LARGE_FONT SCREEN_WIDTH / 70
#define CODE_PADDING SMALL_FONT / 3
#define CONTROLS_SIZE SCREEN_WIDTH / 4
#define DELTA GetFrameTime() * 100
#define SPEED_RATIO ((float)SCREEN_WIDTH / 2560.0)
#define MARK_SPEED 9.0 * SPEED_RATIO
#define CODELINE_SPEED 6.0 * SPEED_RATIO
#define DIALOGUE_LINES 6

typedef struct {Texture2D *a, *b; int x, y, health; float x_multiplier, y_multiplier;} Marks;
typedef struct {char *line;/* Pixel length */ int width, x, y; bool good, active;} CodeLine;


const char *lines[DIALOGUE_LINES] = {
    "Welcome my friend!", 
    "I am Mark Sheldon, lead character of Tufts CS!", 
    "This is the white room.", 
    "Here, you're in control.", 
    "No assignments. No autograders. No debugging.",
    "Just pure low-level programming bliss."
};

char *good_lines[MAXIMUM_CODELINES / 2] = {
    "int i = 5;",
    "if (5 < 7) { printf(\"splendid\"); }",
    "char *goobert = \"goobert\";",
    "int bread = 2;",
    "char f = 'c';",
    "int _string = 60;",
    "int gooberts_money = 5;",
    "int a = 5;",
    "float b = 3.14f;",
    "char c = 'Z';",
    "double d = 2.0;",
    "short f = 10;",
    "unsigned g = 42;",
    "int sum = a + 3;",
    "int diff = a - 2;",
    "int prod = a * 4;",
    "int quo = a / 2;",
    "int mod = a % 2;",
    "a += 1;",
    "b *= 2.0f;",
    "c = 'A';",
    "int arr[3] = {1, 2, 3};",
    "int *p = &a;",
    "int val = *p;",
    "e = e - 500;"
};
char *bad_lines[MAXIMUM_CODELINES / 2] = {
    "int 9lives = 3;",
    "if (a < 5 { printf(\"hi\"); }",
    "char name[5] = 'bob';",
    "float x = (3.14f;",
    "double d = 2.0.0;",
    "unsigned int = 5;",
    "int *p = *a;",
    "char c = 'XY';",
    "int arr[3] = {1, 2,};",
    "long long x = 10LLL;",
    "short s = ;",
    "int a = 5",
    "int sum = a + ;",
    "float f = 1.0f.0f;",
    "int x y = 3;",
    "int z = (a + 3));",
    "int n = sizeof int;",
    "char *str = \"hello;",
    "int m = a *** 2;",
    "return = 5;",
    "int q = (int) ;",
    "int *r = &;",
    "int t = 5,;",
    "double u = 1e;",
    "int v = a + (b *);",
    "goto 100;",
    "int w = 0xG1;"
};

const char *credits = " JumboHack 2026 Project\n Credits: Elias Seidman, Jesse Li, Elijah Greenberg";

void TakeMovementInput();
float Clamp(float v, float _min, float _max);
float Lerp(float a, float b, float interp);
void InitCodelines(CodeLine *codelines[]);
void UpdateCodeLines(CodeLine *codelines[], Sound *good, Sound *bad);
void SpawnCodeLine(CodeLine *codelines);
void DrawCodeLine(CodeLine *codeline, Sound *good, Sound *bad);
void DrawMark(bool red);
void DrawDialogue(int i, float time_since_last);
void GameLoop();
void YouWin();
void YouLose();
void DrawCredits(Color c);
void DrawBackground();

Color DROP_SHADOW = (Color) {0, 0, 0, 100};


int SCREEN_WIDTH;
int SCREEN_HEIGHT;
int NUM_ACTIVE = MAXIMUM_CODELINES;

Font code_font;
Font dialogue_font;
Texture2D backgroundTex;
Marks marks;
int main() 
{
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Bad Marks");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();
    ToggleBorderlessWindowed();
    HideCursor();

    code_font = LoadFont("consolas.ttf");
    dialogue_font = LoadFont("comicsans.ttf");
    Image mark_img = LoadImage("mark.png");
    Image redmark_img = LoadImage("redmark.png");
    Image background = LoadImage("tile.png");

    int scalar = SCREEN_WIDTH / 10;
    ImageResize(&mark_img, SCREEN_WIDTH / 10, SCREEN_WIDTH / 10 * ((float)mark_img.height / (float)mark_img.width));
    ImageResize(&redmark_img, SCREEN_WIDTH / 10, SCREEN_WIDTH / 10 * ((float)mark_img.height / (float)mark_img.width));
    Texture2D markTex = LoadTextureFromImage(mark_img);
    Texture2D redmarkTex = LoadTextureFromImage(redmark_img);
    Texture2D controlsTex = LoadTexture("controls.png");
    Texture2D titleTex = LoadTexture("titlescreen.png");
    backgroundTex = LoadTexture("tile.png");
    SetTextureWrap(backgroundTex, TEXTURE_WRAP_REPEAT);

    marks.a = &markTex;
    marks.b = &redmarkTex;
    marks.x = SCREEN_WIDTH / 2;
    marks.y = SCREEN_HEIGHT / 2;
    marks.health = 3;
    marks.x_multiplier = 0;
    marks.y_multiplier = 0;

    UnloadImage(mark_img);
    UnloadImage(redmark_img);
    SetTargetFPS(GetMonitorRefreshRate(0));
    int i = 0;

    Vector2 text_origin = {0.5, 0};
    float time_since_last = 0;
    float animated_height = 0;
    
    InitAudioDevice();
    Music calm_music = LoadMusicStream("calm.mp3");
    Music title_music = LoadMusicStream("titlescreen.mp3");
    PlayMusicStream(title_music);
    while (!WindowShouldClose()) 
    {
        // animated_height = (sin(GetTime() * 4) * SCREEN_WIDTH / 256);
        UpdateMusicStream(title_music);
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(titleTex, 
            (Rectangle) {0, 0, titleTex.width, titleTex.height},
            (Rectangle) {
                SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + animated_height, CONTROLS_SIZE * 1.35, CONTROLS_SIZE * 1.35
            }, (Vector2) {CONTROLS_SIZE * 1.35 / 2, CONTROLS_SIZE * 1.35 / 2}, 0, RAYWHITE);
        DrawCredits(RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            break;
        EndDrawing();
    }
    StopMusicStream(title_music);
    UnloadMusicStream(title_music);
    PlayMusicStream(calm_music);
    while (!WindowShouldClose()) 
    {
        UpdateMusicStream(calm_music);
        TakeMovementInput();
        
        time_since_last += DELTA;

        // ====================================================================

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBackground();
        DrawTexturePro(controlsTex, 
            (Rectangle) {0, 0, controlsTex.width, controlsTex.height},
            (Rectangle) {
                SCREEN_WIDTH - SCREEN_WIDTH / 24, 0, CONTROLS_SIZE, CONTROLS_SIZE
            }, (Vector2) {CONTROLS_SIZE, 0}, 0, RAYWHITE);
        DrawMark(false);
        if (IsKeyPressed(KEY_ENTER)) {
            i++;
            time_since_last = 0;
        }
        if (i > 5)
        {
            StopMusicStream(calm_music);
            UnloadMusicStream(calm_music);
            EndDrawing();
            GameLoop();
            break;
        }
        DrawDialogue(i, time_since_last);
        EndDrawing();
    }

    UnloadTexture(markTex);
    UnloadTexture(backgroundTex);
    UnloadTexture(controlsTex);
    CloseWindow();
    UnloadFont(dialogue_font);
    UnloadFont(code_font);
    ShowCursor();
    return 0;
}


float Clamp(float v, float _min, float _max) 
{
    return MAX(MIN(_max, v), _min);
}

float Lerp(float a, float b, float interp) 
{
    return a + interp * (b - a);
}

void TakeMovementInput() 
{
    marks.x_multiplier = 0;
    marks.y_multiplier = 0;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) marks.x_multiplier++;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) marks.x_multiplier--;
    if (IsKeyDown(KEY_UP)  || IsKeyDown(KEY_W)) marks.y_multiplier--;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) marks.y_multiplier++;

    // normalize vector
    float magnitude = 
        sqrt(marks.x_multiplier * marks.x_multiplier + 
            marks.y_multiplier * marks.y_multiplier);
    if (magnitude != 0) {
        marks.x_multiplier /= magnitude;
        marks.y_multiplier /= magnitude;
    }

    // move mark
    marks.x += marks.x_multiplier * MARK_SPEED * DELTA;
    marks.y += marks.y_multiplier * MARK_SPEED * DELTA;
    // clamp mark
    marks.x = Clamp(marks.x, 0, SCREEN_WIDTH);
    marks.y = Clamp(marks.y, 0, SCREEN_HEIGHT);
}

float mark_rotation = 0;
Color mark_color = RAYWHITE;
float mark_scale = 1;
void DrawMark(bool red) 
{
    Texture2D *curr_mark; 
    if (red)
        curr_mark = marks.b;
    else
        curr_mark = marks.a;
    
    Rectangle source = {0, 0, marks.a->width, marks.a->height};
    Rectangle destination = {marks.x, marks.y, marks.a->width, curr_mark->height};
    Rectangle shadow_destination = {marks.x + SMALL_FONT, marks.y + 15, marks.a->width, 
        curr_mark->height};
    
    mark_rotation = Lerp(mark_rotation, marks.x_multiplier * 5, 0.12f);
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
//Font *font, int i, int mark_x, int mark_y, int mark_width, int mark_height, float time_since_last
void DrawDialogue(int i, float time_since_last)
{
    time_since_last *= 0.1;

    float animated_font_size = -(time_since_last - 0.75) * (time_since_last - 0.75) + 1.15;
    animated_font_size = Clamp(animated_font_size, 1, 2);
    animated_font_size *= SMALL_FONT;
    // float animated_rotation = Clamp(1 - 0.5 * time_since_last, 0, 100) * 
    //     cos(7 * time_since_last);
    float animated_y = Clamp(1 - 0.5 * time_since_last, 0, 100) * 
        cos(4 * time_since_last);
    animated_y = abs(animated_y);

    //MeasureTextEx(Font font, const char *text, float fontSize, float spacing)
    float text_width = MeasureTextEx(dialogue_font, lines[i], animated_font_size, 0).x;

    DrawTextPro(dialogue_font, lines[i], 
        (Vector2){marks.x, 
            marks.y - marks.a->height / 2 - SCREEN_HEIGHT / 30 + animated_y * 30}, 
        (Vector2){text_width / 2, 0}, 0, animated_font_size, 1, BLACK);
}

void DrawBackground() 
{
    for (int i = 0; i < SCREEN_WIDTH / 16; i++) {
        for (int j = 0; j < SCREEN_HEIGHT / 16; j++) {
            DrawTexturePro(backgroundTex, 
                (Rectangle) {0, 0, backgroundTex.width, backgroundTex.height},
                (Rectangle) {TILE_SIZE * i, TILE_SIZE * j, TILE_SIZE, TILE_SIZE}, 
                (Vector2) {0, 0}, 0, WHITE);
        }
    }
}

void GameLoop()
{
    CodeLine *codelines[MAXIMUM_CODELINES];
    for (int i = 0; i < MAXIMUM_CODELINES; i++) {
        codelines[i] = (CodeLine *)malloc(sizeof(CodeLine));
    }
    InitCodelines(codelines);
    Music game_music = LoadMusicStream("music.mp3");
    PlayMusicStream(game_music);
    Sound pickup_good = LoadSound("pickupgood.mp3");
    Sound pickup_bad = LoadSound("pickupbad.mp3");
    Texture2D objectiveTex = LoadTexture("objective.png");
    const char *game_message = "Defend Mark from the invalid syntaxes!\n     Be sure to pick up the valid syntax\n         Don't let his health go NULL.";
    while (!WindowShouldClose())
    {
        TakeMovementInput();
        
        UpdateMusicStream(game_music);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBackground();
        UpdateCodeLines(codelines, &pickup_good, &pickup_bad);
        DrawMark(true);

        if (marks.health < 0) {
            EndDrawing();
            YouLose();
            break;
        }

        if (GetTime() < 30) 
            // DrawTextEx(*font, game_message, 
            // (Vector2) {SCREEN_WIDTH / 2 - (MeasureText(game_message, SMALL_FONT) / 2),
            // SCREEN_HEIGHT / 16}, SMALL_FONT, 0, BLACK);
            DrawTexturePro(objectiveTex, 
                (Rectangle) {0, 0, objectiveTex.width, objectiveTex.height},
                (Rectangle) {
                    SCREEN_WIDTH / 2, 
                    SCREEN_WIDTH / 28 * (1 + sin(GetTime() * 4) * 0.05), 
                    CONTROLS_SIZE, CONTROLS_SIZE
                }, (Vector2) {CONTROLS_SIZE / 2, 0}, 0, RAYWHITE);

        if (marks.health >= 0)
            DrawTextEx(code_font, TextFormat("HEALTH = %d", marks.health), 
            (Vector2) {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 16}, LARGE_FONT, 0, BLACK);

        if (NUM_ACTIVE <= 1) {
            EndDrawing();
            YouWin();
            break;
        }

        EndDrawing();
    }

    for (int i = 0; i < MAXIMUM_CODELINES; i++)
        free(codelines[i]);
    StopMusicStream(game_music);
    UnloadMusicStream(game_music);
    UnloadTexture(objectiveTex);
}

void InitCodelines(CodeLine *codelines[])
{
    for (int i = 0; i < MAXIMUM_CODELINES / 2; i++) {
        srand(time(NULL) + i);
        
        codelines[i]->line = good_lines[i];
        codelines[i]->width = MeasureTextEx(code_font, codelines[i]->line, 30, 0).x;
        codelines[i]->x = GetRandomValue(-1 * MAXIMUM_CODELINES / 2 * SCREEN_WIDTH, 0);
        codelines[i]->y = GetRandomValue(SCREEN_HEIGHT / 8, 7 * SCREEN_HEIGHT / 8);
        codelines[i]->active = true;
        codelines[i]->good = true;
    }

    for (int i = MAXIMUM_CODELINES / 2; i < MAXIMUM_CODELINES - 1; i++) {
        srand(time(NULL) + i);

        codelines[i]->line = bad_lines[i - (MAXIMUM_CODELINES / 2)];
        codelines[i]->width = MeasureTextEx(code_font, codelines[i]->line, 30, 0).x;
        codelines[i]->x = GetRandomValue(-1 * MAXIMUM_CODELINES / 2 * SCREEN_WIDTH, 0);
        codelines[i]->y = GetRandomValue(0, SCREEN_HEIGHT - 30);
        codelines[i]->active = true;
        codelines[i]->good = false;
    }
}

void UpdateCodeLines(CodeLine *codelines[], Sound *good, Sound *bad)
{
    for (int i = 0; i < MAXIMUM_CODELINES; i++) {
        CodeLine *cur = codelines[i];
        cur->x += CODELINE_SPEED * DELTA;
        if (codelines[i]->active)
            DrawCodeLine(cur, good, bad);
    }
}

void DrawCodeLine(CodeLine *codeline, Sound *good, Sound *bad)
{
    DrawRectangle(
        codeline->x - CODE_PADDING, codeline->y, codeline->width + CODE_PADDING, SMALL_FONT, BLACK);
    DrawRectangle(
        codeline->x - CODE_PADDING + 12, codeline->y + 12, codeline->width + CODE_PADDING, SMALL_FONT, DROP_SHADOW);
    DrawTextEx(code_font, codeline->line, 
        (Vector2) {codeline->x, codeline->y}, SMALL_FONT, 0, RAYWHITE);

    if (codeline->x >= SCREEN_WIDTH) {
        if (codeline->good) {
            marks.health--;
            PlaySound(*bad);
        }
        codeline->active = false;
        NUM_ACTIVE--;
        return;
    }
    BoundingBox codeline_bounds;
    // codeline_bounds.min = (Vector3){codeline->x - 6, codeline->y, -100};
    // codeline_bounds.max = (Vector3){codeline->x + codeline->width + 6, codeline->y + 30, 100};
    // bool collision = CheckCollisionBoxSphere(codeline_bounds, 
    //     (Vector3){marks.x, marks.y, 0}, marks.a)
    bool collision = CheckCollisionRecs(
        (Rectangle) {codeline->x - 6, codeline->y, codeline->width + 12, SMALL_FONT}, 
        (Rectangle) {marks.x - marks.b->width / 2, marks.y - marks.b->height / 2, marks.b->width, marks.b->height});
    if (collision && !codeline->good) {
        PlaySound(*bad);
        marks.health -= 2;
        codeline->x = 0;
        codeline->y = SCREEN_WIDTH * -2;
        codeline->active = false;
        NUM_ACTIVE--;
        mark_color = RED;
    }
    else if (collision && codeline->good) {
        PlaySound(*good);
        marks.health++;
        codeline->active = false;
        NUM_ACTIVE--;
        mark_color = GREEN;
    }

    //CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius);
    //
    // CheckCollisionBoxSphere()
}

void YouWin()
{
    Texture2D win_logo = LoadTexture("win.png");
    Sound win_sound = LoadSound("win.mp3");
    PlaySound(win_sound);
    float animated_height = 0;
    while (!WindowShouldClose() && GetCharPressed() == 0)
    {
        animated_height = (sin(GetTime() * 4) * SCREEN_WIDTH / 256);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBackground();
        DrawTextEx(code_font, TextFormat("HEALTH = %d", marks.health), 
            (Vector2) {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 16}, LARGE_FONT, 0, (Color){0x8d, 0xc6, 0x3f, 0xff});
        DrawTexturePro(win_logo, 
            (Rectangle) {0, 0, win_logo.width, win_logo.height},
            (Rectangle) {
                SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + animated_height, CONTROLS_SIZE * 2, CONTROLS_SIZE * 2
            }, (Vector2) {CONTROLS_SIZE, CONTROLS_SIZE}, 0, RAYWHITE);
        DrawCredits((Color){0x8d, 0xc6, 0x3f, 0xff});
        EndDrawing();
    }
    
    UnloadTexture(win_logo);
    UnloadSound(win_sound);
}

void YouLose()
{
    Texture2D lose_logo = LoadTexture("lose.png");
    Sound lose_sound = LoadSound("lose.mp3");
    PlaySound(lose_sound);
    float animated_x = 0;
    float scale = 1;
    while(!WindowShouldClose() && GetCharPressed() == 0) {
        animated_x = (sin(GetTime() * 30) * SCREEN_WIDTH / 1024);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBackground();
        DrawTexturePro(lose_logo, 
            (Rectangle) {0, 0, lose_logo.width, lose_logo.height},
            (Rectangle) {
                SCREEN_WIDTH / 2 + animated_x, SCREEN_HEIGHT / 2, 
                CONTROLS_SIZE * 2 * scale, CONTROLS_SIZE * 2 * scale
            }, (Vector2) {CONTROLS_SIZE * scale, CONTROLS_SIZE * scale}, 0, RAYWHITE);
        DrawTextEx(code_font, "HEALTH = NULL", (Vector2) 
            {SCREEN_WIDTH - SCREEN_WIDTH / 8, 
            SCREEN_HEIGHT / 20}, LARGE_FONT, 0, (Color){0xbe, 0x1e, 0x2d, 0xff});
        DrawCredits((Color){0xbe, 0x1e, 0x2d, 0xff});
        EndDrawing();
    }

    UnloadTexture(lose_logo);
    UnloadSound(lose_sound);
}

void DrawCredits(Color c)
{
     DrawTextEx(dialogue_font, credits, (Vector2) 
            {SCREEN_WIDTH / 10, 
            SCREEN_HEIGHT - SCREEN_HEIGHT / 10}, LARGE_FONT, 0, c);
}