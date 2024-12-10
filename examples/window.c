
#include <stdio.h>
#define SOKOL_APP_IMPL
#include "sokol_app.h"
#include "../src/buder.h"
#include "sokol_glue.h"

static buder_t buder;

static void init(void)
{
    buder_init(&buder);
}

static void frame(void)
{
    buder_set_size(&buder, sapp_width(), sapp_height());
    buder_begin_frame(&buder);
    {
        // Unos rectangulos de diferentes colores y tamaños, los borders son diferentes colores.
        buder_draw_rect(100, 100, 100, 100, (buder_color_t){255, 0, 0, 255}, (buder_color_t){0, 0, 255, 255}, 5, 0);
        buder_draw_rect(200, 200, 100, 100, (buder_color_t){0, 255, 0, 255}, (buder_color_t){255, 0, 0, 255}, 10, 0);

        // Unas lineas cortas de diferentes colores
        buder_draw_line(0, 0, 100, 100, 1, (buder_color_t){255, 0, 0, 255}, 0);
        buder_draw_line(0, 0, 100, 200, 5, (buder_color_t){0, 255, 0, 255}, 0);

        // Unos circulos de diferentes colores, , los borders son diferentes colores.
        buder_draw_circle(500, 200, 50, (buder_color_t){0, 0, 255, 255}, (buder_color_t){255, 0, 0, 255}, 5, 0);
        buder_draw_circle(600, 200, 50, (buder_color_t){255, 0, 0, 255}, (buder_color_t){0, 255, 0, 255}, 1, 0);

        // Unos triangulos de diferentes colores [azul, rojo], uno del lado del otro.
        buder_draw_triangle(500, 500, 600, 500, 550, 600, (buder_color_t){0, 0, 255, 255}, 0);
        buder_draw_triangle(600, 500, 700, 500, 650, 600, (buder_color_t){255, 0, 0, 255}, 0);
    }
    buder_end_frame(&buder);
}

static void cleanup(void)
{
    buder_shutdown(&buder);
}

sapp_desc sokol_main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "window example",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}

