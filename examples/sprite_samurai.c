#include "../src/buder.h"

typedef enum samuraiState
{
    IDLE,
    RUN,
    ATTACK,
    HURT,
    SAMURAI_STATE_NUM
} samuraiState;

static const char *samuraiStateName[] = {"IDLE", "RUN", "ATTACK", "HURT"};

static buder_sprite_animation_t SamuraiAnimation = {0};

void bwindow_init(buder_t *buder)
{
    // bdr_viewport_color(&buder, (buder_color_t){245, 245, 245, 255});
    bdr_sprite_animation_register(&SamuraiAnimation, IDLE, (buder_sprite_animation_frame_t){.hframe = 10, .vframe = 1, .loop = true, .texture = bdr_load_texture("resources/samurai/IDLE.png")});
    bdr_sprite_animation_register(&SamuraiAnimation, RUN, (buder_sprite_animation_frame_t){.hframe = 16, .vframe = 1, .loop = true, .texture = bdr_load_texture("resources/samurai/RUN.png")});
    bdr_sprite_animation_register(&SamuraiAnimation, ATTACK, (buder_sprite_animation_frame_t){.hframe = 7, .vframe = 1, .texture = bdr_load_texture("resources/samurai/ATTACK1.png")});
    bdr_sprite_animation_register(&SamuraiAnimation, HURT, (buder_sprite_animation_frame_t){.hframe = 4, .vframe = 1, .texture = bdr_load_texture("resources/samurai/HURT.png")});
    bdr_sprite_animation_play(&SamuraiAnimation, IDLE);
    bdr_sprite_animation_set_speed(&SamuraiAnimation, 8);
}

void bwindow_frame(buder_t *buder, float delta)
{
    // begin update -- -- --
    if (bdr_keyboard_is_pressed(buder, BUDER_KEY_1))
        bdr_sprite_animation_play(&SamuraiAnimation, IDLE);
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_2))
        bdr_sprite_animation_play(&SamuraiAnimation, RUN);
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_3))
        bdr_sprite_animation_play(&SamuraiAnimation, ATTACK);
    else if (bdr_keyboard_is_pressed(buder, BUDER_KEY_4))
        bdr_sprite_animation_play(&SamuraiAnimation, HURT);

    buder_rect_t samurai_rect = bdr_sprite_animation_update(&SamuraiAnimation);

    // begin draw -- -- --
    {
        bdr_camera_begin((buder_camera_t){.zoom = 3.0f});
        bdr_draw_texture_rect(bdr_sprite_animation_get_texture(&SamuraiAnimation), samurai_rect, (buder_vec2_t){0.0f, 0.0f}, WHITE, 0);
        bdr_camera_end();

        bdr_draw_text("Press 1 IDLE, 2 RUN, 3 ATTACK y 4 HURT", 10, 30, 20, WHITE, 0);
        bdr_draw_text(bdr_string_format("Animation Active: %s", samuraiStateName[SamuraiAnimation.active]), 10, 60, 20, PINK, 0);
    }
    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    bdr_sprite_animation_destroy(&SamuraiAnimation);
}
