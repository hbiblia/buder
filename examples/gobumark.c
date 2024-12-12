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

void spawn_gobus(Gobu *gobus, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (gobuesCount < MAX_GOBUS)
        {
            gobus[gobuesCount].position = buder_get_mouse_position();
            gobus[gobuesCount].velocity = (buder_vec2_t){(float)buder_random_int(-250, 250) / 60.0f, (float)buder_random_int(-250, 250) / 60.0f};
            gobus[gobuesCount].color = (buder_color_t){buder_random_int(50, 240), buder_random_int(50, 240), buder_random_int(50, 240), 255};
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
    gobumark = buder_load_texture("resources/gobu_small.png");
    buder_set_stage_color(buder, BUDEWHITE);
}

void bwindow_frame(buder_t *buder, float delta)
{
    if (buder_mouse_is_button_down(BUDER_MOUSEBUTTON_LEFT))
    {
        spawn_gobus(gobus, SPAWN_GOBUS);
    }

    update_gobues(buder->width, buder->height);

    buder_begin_frame(buder);
    {
        for (int i = 0; i < gobuesCount; i++)
        {
            buder_draw_texture(gobumark, BUDER_RECT_ZERO, (buder_rect_t){gobus[i].position.x, gobus[i].position.y, 0, 0},
                               (buder_vec2_t){1, 1}, (buder_vec2_t){0.0f, 0.0f}, 0, gobus[i].color, 0);
        }

        buder_draw_rect(0, 0, buder->width, 40, BLACK, BLANK, 0, 0);
        buder_draw_textd(buder_string_format("FPS: %d", 0), 10, 30, 30, DARKGREEN, 0); // proximamente buder_get_fps()
        buder_draw_textd(buder_string_format("bunnies: %i", gobuesCount), 140, 30, 30, GREEN, 0);
        buder_draw_textd(buder_string_format("batched draw calls: %i", 1 + gobuesCount / MAX_BATCH_ELEMENTS), 340, 30, 30, MAROON, 0);
    }
    buder_end_frame(buder);
}

void bwindow_shutdown(void)
{
    free(gobus);
    buder_free_texture(gobumark);
}

void bwindow_event(buder_t *buder, const buder_event_t *event)
{
    // Handle mouse events
    buder_set_mouse_position(event->mouse_x, event->mouse_y);
    buder_mouse_button_events(event->mouse_button, event->type);
    // Handle keyboard events
}
