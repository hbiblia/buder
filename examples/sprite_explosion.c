#include "../src/buder.h"

static buder_sprite_animation_t button_animation = {0};
static buder_texture_t button;
static buder_vec2_t mousePos = {0};
static bool isExplosionSpawned = false;

void bwindow_init(buder_t *buder)
{
    button = bdr_load_texture("resources/explosion.png");
    bdr_sprite_animation_register(&button_animation, 0, (buder_sprite_animation_frame_t){.hframe = 5, .vframe = 5, .texture = button});
}

void bwindow_frame(buder_t *buder, float delta)
{
    // begin update -- -- --
    if ((bdr_mouse_is_pressed(buder, BUDER_MOUSEBUTTON_LEFT) || bdr_mouse_is_pressed(buder, BUDER_MOUSEBUTTON_RIGHT)) && !isExplosionSpawned)
    {
        mousePos = bdr_mouse_get_pos(buder);
        buder_vec2_t size = bdr_sprite_animation_get_size_frame(&button_animation);
        mousePos.x -= size.x / 2;
        mousePos.y -= size.y / 2;

        bdr_sprite_animation_play(&button_animation, 0);
        bdr_play_sound("resources/boom.wav");
        isExplosionSpawned = true;
    }

    if (bdr_sprite_animation_is_finished(&button_animation))
        isExplosionSpawned = false;

    // begin draw -- -- --
    if (isExplosionSpawned)
    {
        bdr_draw_texture_rect(
            bdr_sprite_animation_get_texture(&button_animation),
            bdr_sprite_animation_update(&button_animation),
            mousePos, WHITE, 0);
    }

    const char *instructions = "Sprite Explosion Click";
    const int t1_width = bdr_text_measure(bdr_get_default_font(), instructions, 20);
    bdr_draw_text(instructions, buder->width / 2 - t1_width / 2, buder->height / 2 - 40, 20, WHITE, 0);

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    bdr_free_texture(button);
    bdr_sprite_animation_destroy(&button_animation);
}
