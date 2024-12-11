#include "../src/buder.h"

static buder_image_t gobu_texture;
static float angle = 0;

void bwindow_init(buder_t *buder)
{
    // buder_set_stage_color(&buder, (buder_color_t){245, 245, 245, 255});
    gobu_texture = buder_load_texture("resources/gobu.png");
}

void bwindow_frame(buder_t *buder, float delta)
{
    angle += 3.0f * delta;

    buder_set_stage_size(buder, buder->width, buder->height);
    buder_begin_frame(buder);
    {
        // Unos rectangulos de diferentes colores y tamaños, los borders son diferentes colores.
        buder_draw_rect(100, 100, 100, 100, PINK, RED, 5, 0);
        buder_draw_rect(200, 200, 100, 100, MAROON, PINK, 10, 0);

        // Unas lineas cortas de diferentes colores
        buder_draw_line(0, 0, 100, 100, 1, PINK, 0);
        buder_draw_line(0, 0, 100, 200, 5, LIME, 0);

        // Unos circulos de diferentes colores, , los borders son diferentes colores.
        buder_draw_circle(500, 200, 50, GREEN, DARKGREEN, 5, 0);
        buder_draw_circle(620, 200, 50, LIME, GREEN, 1, 0);

        // Unos triangulos de diferentes colores [azul, rojo], uno del lado del otro.
        buder_draw_triangle(500, 480, 600, 480, 550, 580, SKYBLUE, 0);
        buder_draw_triangle(600, 480, 700, 480, 650, 580, BLUE, 0);

        // mostrar una textura en la pantalla
        buder_draw_texture(gobu_texture, (buder_rect_t){0, 0, 0, 0}, (buder_rect_t){380, 250, 0, 0},
                           (buder_vec2_t){1, 1}, (buder_vec2_t){gobu_texture.width / 2, gobu_texture.height / 2}, angle, 0);
    }
    buder_end_frame(buder);
}

void bwindow_shutdown(void)
{
    buder_texture_free(gobu_texture);
}

void bwindow_event(buder_t *buder, const buder_event_t *event){}
