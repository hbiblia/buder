#include "../src/buder.h"

#define MAX_GOBUS 10800
#define MAX_BATCH_ELEMENTS 8192
#define SPAWN_GOBUS 50

typedef struct Gobu
{
    buder_vec2_t position;
    buder_vec2_t velocity;
    buder_color_t color;
} Gobu;

static buder_texture_t gobumark;
static int gobuesCount = 0;
static Gobu *gobus;

void spawn_gobus(buder_t *buder, Gobu *gobus, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (gobuesCount < MAX_GOBUS)
        {
            gobus[gobuesCount].position = bdr_mouse_get_pos(buder);
            gobus[gobuesCount].velocity = (buder_vec2_t){(float)bdr_math_random_int(-250, 250) / 60.0f, (float)bdr_math_random_int(-250, 250) / 60.0f};
            gobus[gobuesCount].color = (buder_color_t){bdr_math_random_int(50, 240), bdr_math_random_int(50, 240), bdr_math_random_int(50, 240), 255};
            gobuesCount++;
        }
    }
}

void update_gobues(int width, int height)
{
    for (int i = 0; i < gobuesCount; i++)
    {
        gobus[i].position.x += gobus[i].velocity.x;
        gobus[i].position.y += gobus[i].velocity.y;

        if (((gobus[i].position.x + gobumark.width / 2) > width) ||
            ((gobus[i].position.x + gobumark.width / 2) < 0))
            gobus[i].velocity.x *= -1;
        if (((gobus[i].position.y + gobumark.height / 2) > height) ||
            ((gobus[i].position.y + gobumark.height / 2 - 40) < 0))
            gobus[i].velocity.y *= -1;
    }
}

void bwindow_init(buder_t *buder)
{
    gobus = (Gobu *)malloc(MAX_GOBUS * sizeof(Gobu));
    gobumark = bdr_load_texture("resources/gobu_small.png");
    bdr_viewport_color(buder, BUDEWHITE);
}

void bwindow_frame(buder_t *buder, float delta)
{
    if (bdr_mouse_is_down(buder, BUDER_MOUSEBUTTON_LEFT))
    {
        spawn_gobus(buder, gobus, SPAWN_GOBUS);
    }

    update_gobues(buder->width, buder->height);

    for (int i = 0; i < gobuesCount; i++)
    {
        bdr_draw_texture(gobumark,(buder_vec2_t){gobus[i].position.x, gobus[i].position.y}, gobus[i].color, 0);
    }

    bdr_draw_rect(0, 0, buder->width, 40, BLACK, BLANK, 0, 0);
    bdr_draw_text(bdr_string_format("FPS: %d", 0), 10, 25, 25, DARKGREEN, 0); // proximamente buder_get_fps()
    bdr_draw_text(bdr_string_format("bunnies: %i", gobuesCount), 135, 25, 25, GREEN, 0);
    bdr_draw_text(bdr_string_format("batched draw calls: %i", 1 + gobuesCount / MAX_BATCH_ELEMENTS), 345, 25, 25, MAROON, 0);

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    free(gobus);
    bdr_free_texture(gobumark);
}
