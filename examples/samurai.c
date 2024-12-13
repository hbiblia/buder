#include "../src/buder.h"

typedef enum samuraiState
{
    IDLE,
    RUN,
    ATTACK,
    HURT,
    SAMURAI_STATE_NUM
} samuraiState;

typedef struct
{
    int hframe;
    int vframe;
    int frame;
} spriteAnimation;

static int state_active = IDLE;
static const char *samuraiStateName[] = {"IDLE", "RUN", "ATTACK", "HURT"};
static buder_texture_t samurai_texture[SAMURAI_STATE_NUM] = {0};
static spriteAnimation samurai_animation[SAMURAI_STATE_NUM] = {0};

static int framesCounter = 0;
static int framesSpeed = 8;

buder_rect_t animation_update(void)
{
    framesCounter++;
    if (framesCounter >= (60 / framesSpeed))
    {
        framesCounter = 0;
        samurai_animation[state_active].frame++;
        if (samurai_animation[state_active].frame > samurai_animation[state_active].hframe)
        {
            samurai_animation[state_active].frame = 0;
        }
    }

    int twidth = samurai_texture[state_active].width;
    int theight = samurai_texture[state_active].height;

    int frame_w = twidth / samurai_animation[state_active].hframe;
    int frame_h = theight / samurai_animation[state_active].vframe;

    float frame_x = (samurai_animation[state_active].frame % (int)(twidth / frame_w)) * frame_w;
    float frame_y = (samurai_animation[state_active].frame / (twidth / frame_h)) * frame_h;

    buder_rect_t samurai_rect = (buder_rect_t){frame_x, frame_y, frame_w, frame_h};

    return samurai_rect;
}

void bwindow_init(buder_t *buder)
{
    // bdr_viewport_color(&buder, (buder_color_t){245, 245, 245, 255});
    samurai_texture[IDLE] = bdr_load_texture("resources/samurai/IDLE.png");
    samurai_animation[IDLE] = (spriteAnimation){10, 1, 0};

    samurai_texture[RUN] = bdr_load_texture("resources/samurai/RUN.png");
    samurai_animation[RUN] = (spriteAnimation){16, 1, 0};

    samurai_texture[ATTACK] = bdr_load_texture("resources/samurai/ATTACK 1.png");
    samurai_animation[ATTACK] = (spriteAnimation){7, 1, 0};

    samurai_texture[HURT] = bdr_load_texture("resources/samurai/HURT.png");
    samurai_animation[HURT] = (spriteAnimation){4, 1, 0};
}

void bwindow_frame(buder_t *buder, float delta)
{
    // begin update -- -- --
    if (bdr_keyboard_is_pressed(buder, BUDER_KEY_1))
        state_active = IDLE;
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_2))
        state_active = RUN;
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_3))
        state_active = ATTACK;
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_4))
        state_active = HURT;

    // begin draw -- -- --
    {
        bdr_camera_begin((buder_camera_t){.zoom = 3.0f});
        bdr_draw_texture_rect(samurai_texture[state_active], animation_update(), (buder_vec2_t){0.0f, 0.0f}, WHITE, 0);
        bdr_camera_end();

        bdr_draw_text("Press 1 IDLE, 2 RUN, 3 ATTACK y 4 HURT", 10, 30, 20, WHITE, 0);
        bdr_draw_text(bdr_string_format("Animation Active: %s", samuraiStateName[state_active]), 10, 60, 20, PINK, 0);
    }
    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    for (int i = 0; i < SAMURAI_STATE_NUM; i++)
        bdr_free_texture(samurai_texture[i]);
}
