#include "engine.h"
#include "raylib.h"

#include <stdlib.h>

#ifdef WEB
    #include <emscripten.h>
#endif

#define TRANSITION_FRAMES   5

static Texture transition_texture = { 0 };
static Rectangle transition_texture_rec = { 0 };
static Texture back_texture = { 0 };
static Rectangle back_texture_rec = { 0 };
static Texture delete_texture = { 0 };
static Rectangle delete_texture_rec = { 0 };

void init_game(Game *game)
{
    game->current_screen = SCREEN_WAIT;
    game->mode = MODE_SURVIVAL;
    game->title_ss = SS_MAIN;
    game->bg_chosen = 0;
    game->wait_situation = WS_LOGO;

#ifdef WEB
    emsave("mode_survival", true);
    emsave("bg_0", true);
    emsave_f("master_volume", 1.0f);
    emsave_f("sound_volume", 1.0f);
    emsave_f("music_volume", 1.0f);
#else
    game->unlocked_modes[MODE_SURVIVAL] = true;
    game->unlocked_modes[MODE_INFINITE] = false;
    game->unlocked_modes[MODE_SANDBOX] = false;
    game->unlocked_modes[MODE_INSANE] = false;
    game->unlocked_bg[0] = true;
    game->unlocked_bg[1] = false;
    game->unlocked_bg[2] = false;
    game->unlocked_bg[3] = false;
    game->volume = 1.0f;
    game->sound_volume = 1.0f;
    game->music_volume = 1.0f;
#endif
    game->frames_counter = 0;
    transition_texture = LoadTexture("assets/transition_animation.png");
    transition_texture_rec = (Rectangle){ 0.0f, 0.0f, transition_texture.width/TRANSITION_FRAMES, transition_texture.height };
    back_texture = LoadTexture("assets/button_back.png");
    back_texture_rec = (Rectangle){ 0.0f, 0.0f, 240.0f, 64.0f };
    delete_texture = LoadTexture("assets/button_delete.png");
    delete_texture_rec = (Rectangle){ 0.0f, 0.0f, 240.0f, 64.0f };
}

void deinit_game(Game *game)
{
    UnloadTexture(transition_texture);
    UnloadTexture(back_texture);
    UnloadTexture(delete_texture);
}

void transition_out(Game *game)
{
    // big to small
    transition_texture_rec.y = 0.0f;
    game->frames_counter++;

    if (game->frames_counter >= 10) {
        transition_texture_rec.x += transition_texture.width/TRANSITION_FRAMES;
        game->frames_counter = 0;
    }

    DrawTextureRec(transition_texture, transition_texture_rec, (Vector2){ 0.0f, 0.0f }, WHITE);
}

void transition_zero(void)
{
    transition_texture_rec.x = 0.0f;
}

bool transition_done(void)
{
    return (transition_texture_rec.x == transition_texture.width);
}

bool update_back_button(int x, int y, int width, int height)
{
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, width, height })) {
        back_texture_rec.x = 240.0f;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    } else {
        back_texture_rec.x = 0.0f;
    }

    return false;
}

void draw_back_button(int x, int y)
{
    DrawTextureRec(back_texture, back_texture_rec, (Vector2){ x, y }, WHITE);
}

bool update_delete_button(int x, int y, int width, int height)
{
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ x, y, width, height })) {
        delete_texture_rec.x = 240.0f;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    } else {
        delete_texture_rec.x = 0.0f;
    }

    return false;
}

void draw_delete_button(int x, int y)
{
    DrawTextureRec(delete_texture, delete_texture_rec, (Vector2){ x, y }, WHITE);
}

void emsave(const char *key, int value)
{
#ifdef WEB
    emscripten_run_script(TextFormat("localStorage.setItem(\"%s\", %d)", key, value));
#endif
}

int emload(const char *key)
{
#ifdef WEB
    const char *script = TextFormat("localStorage.getItem(\"%s\")", key);
    char *result = emscripten_run_script_string(script);

    return atoi(result);
#endif

    return 0;
}

void emsave_f(const char *key, float value)
{
#ifdef WEB
    emscripten_run_script(TextFormat("localStorage.setItem(\"%s\", %f)", key, value));
#endif
}

float emload_f(const char *key)
{
#ifdef WEB
    const char *script = TextFormat("localStorage.getItem(\"%s\")", key);
    char *result = emscripten_run_script_string(script);

    return (float)atof(result);
#endif

    return 0.0f;
}

