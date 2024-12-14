#include "../src/buder.h"

typedef enum
{
    BUTTON_IDLE,
    BUTTON_HOVER,
    BUTTON_PRESSED
} button_state_t;

static buder_sprite_animation_t button_animation = {0};
static buder_texture_t button;

void bwindow_init(buder_t *buder)
{
    button = bdr_load_texture("resources/button.png");

    bdr_sprite_animation_register(&button_animation, 0, (buder_sprite_animation_frame_t){.hframe = 1, .vframe = 3, .texture = button});

}

void bwindow_frame(buder_t *buder, float delta)
{
    // begin update -- -- --
    buder_vec2_t buttonFrameSize = bdr_sprite_animation_get_size_frame(&button_animation);
    buder_rect_t buttonBounds = (buder_rect_t){buder->width / 2 - buttonFrameSize.x / 2, buder->height / 2 - buttonFrameSize.y / 2, buttonFrameSize.x, buttonFrameSize.y};

    if (bdr_collision_point_rec(bdr_mouse_get_pos(buder), buttonBounds))
    {
        bdr_sprite_animation_set_frame(&button_animation, bdr_mouse_is_down(buder, BUDER_MOUSEBUTTON_LEFT) ? BUTTON_PRESSED : BUTTON_HOVER);
        if (bdr_mouse_is_released(buder, BUDER_MOUSEBUTTON_LEFT))
            bdr_play_sound("resources/buttonfx.wav"); // experimental
    }
    else
    {
        bdr_sprite_animation_set_frame(&button_animation, BUTTON_IDLE);
    }

    // begin draw -- -- --

    bdr_draw_texture_rect(
        bdr_sprite_animation_get_texture(&button_animation),
        bdr_sprite_animation_update(&button_animation),
        (buder_vec2_t){buttonBounds.x, buttonBounds.y}, WHITE, 0);

    const char* instructions = "Sprite Button Click";
    const int t1_width = bdr_text_measure(bdr_get_default_font(), instructions, 20);
    bdr_draw_text(instructions, buder->width/2-t1_width/2, buder->height/2-60, 20, WHITE, 0);

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    bdr_free_texture(button);
    bdr_sprite_animation_destroy(&button_animation);
}
