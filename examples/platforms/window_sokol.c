
#include <stdio.h>
#define SOKOL_APP_IMPL
#include "sokol_app.h"
#include "../src/buder.h"
#include "sokol_glue.h"

static buder_t buder;

void bwindow_init(buder_t *buder);
void bwindow_frame(buder_t *buder, float delta);
void bwindow_event(buder_t *buder, const buder_event_t *event);
void bwindow_shutdown(void);

static void init(void)
{
    buder_init(&buder, sapp_width(), sapp_height());
    bwindow_init(&buder);
}

static void frame(void)
{
    bwindow_frame(&buder, sapp_frame_duration());
}

static void eventcb(const sapp_event *event)
{
    bdr_event_pool(&buder, &(const buder_event_t){
        .mouse_x = event->mouse_x,
        .mouse_y = event->mouse_y,
        .mouse_dx = event->mouse_dx,
        .mouse_dy = event->mouse_dy,
        .scroll_x = event->scroll_x,
        .scroll_y = event->scroll_y,
        .type = (buder_event_type)event->type,
        .key = event->key_code,
        .char_code = event->char_code,
        .mouse_button = (buder_mousebutton)event->mouse_button,
    });
}

static void cleanup(void)
{
    bwindow_shutdown();
    buder_shutdown(&buder);
}

sapp_desc sokol_main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = eventcb,
        .width = 800,
        .height = 600,
        .window_title = "Example Buder + SokolApp",
        .icon.sokol_default = true,
    };
}
