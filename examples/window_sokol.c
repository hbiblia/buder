
#include <stdio.h>
#define SOKOL_APP_IMPL
#include "sokol_app.h"
#include "../src/buder.h"
#include "sokol_glue.h"

static buder_t buder;
static buder_image_t gobu_texture;
static float angle = 0;

static void init(void)
{
    buder_init(&buder);
    // buder_set_stage_color(&buder, (buder_color_t){245, 245, 245, 255});
    gobu_texture = buder_load_texture("resources/gobu.png");
}

static void frame(void)
{
    angle += 3.0f * sapp_frame_duration();

    buder_set_stage_size(&buder, sapp_width(), sapp_height());
    buder_begin_frame(&buder);
    {
        // Unos rectangulos de diferentes colores y tamaños, los borders son diferentes colores.
        buder_draw_rect(100, 100, 100, 100, (buder_color_t){102, 191, 255, 255}, (buder_color_t){0, 121, 241, 255}, 5, 0);
        buder_draw_rect(200, 200, 100, 100, (buder_color_t){0, 228, 48, 255}, (buder_color_t){0, 158, 47, 255}, 10, 0);

        // Unas lineas cortas de diferentes colores
        buder_draw_line(0, 0, 100, 100, 1, (buder_color_t){255, 109, 194, 255}, 0);
        buder_draw_line(0, 0, 100, 200, 5, (buder_color_t){230, 41, 55, 255}, 0);

        // Unos circulos de diferentes colores, , los borders son diferentes colores.
        buder_draw_circle(500, 200, 50, (buder_color_t){0, 0, 255, 255}, (buder_color_t){112, 31, 126, 255}, 5, 0);
        buder_draw_circle(620, 200, 50, (buder_color_t){255, 0, 0, 255}, (buder_color_t){211, 176, 131, 255}, 1, 0);

        // Unos triangulos de diferentes colores [azul, rojo], uno del lado del otro.
        buder_draw_triangle(500, 480, 600, 480, 550, 580, (buder_color_t){0, 82, 172, 255}, 0);
        buder_draw_triangle(600, 480, 700, 480, 650, 580, (buder_color_t){135, 60, 190, 255}, 0);

        // mostrar una textura en la pantalla
        buder_draw_texture(gobu_texture, (buder_rect_t){0, 0, 0, 0}, (buder_rect_t){380, 250, 0, 0},
                           (buder_vec2_t){1, 1}, (buder_vec2_t){gobu_texture.width / 2, gobu_texture.height / 2}, angle, 0);
    }
    buder_end_frame(&buder);
}

static void cleanup(void)
{
    buder_texture_free(gobu_texture);
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
        .width = 800,
        .height = 600,
        .window_title = "window example",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
