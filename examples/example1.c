#include "../src/buder.h"

static buder_texture_t gobu_texture;
static buder_font_t fontDefault;
static buder_camera_t camera = {0};
static float angle = 0;

void bwindow_init(buder_t *buder)
{
    // bdr_viewport_color(&buder, (buder_color_t){245, 245, 245, 255});
    gobu_texture = bdr_load_texture("resources/gobu.png");
    fontDefault = bdr_load_font("resources/Roboto-Regular.ttf");
}

void bwindow_frame(buder_t *buder, float delta)
{
    angle += 3.0f * delta;

    // zoom sube y baja
    camera.zoom = 1.0f + sinf(angle) * 0.05f;

    // begin draw -- -- --
    bdr_draw_grid(buder->width, buder->height, 20, GRIDBLACK, 0);

    bdr_camera_begin(camera);
    {
        // Unos rectangulos de diferentes colores y tamaños, los borders son diferentes colores.
        bdr_draw_rect(100, 100, 100, 100, PINK, RED, 5, 0);
        bdr_draw_rect(200, 200, 100, 100, MAROON, PINK, 10, 0);

        // Unas lineas cortas de diferentes colores
        bdr_draw_line(0, 0, 100, 100, 1, PINK, 0);
        bdr_draw_line(0, 0, 100, 200, 5, LIME, 0);

        // Unos circulos de diferentes colores, , los borders son diferentes colores.
        bdr_draw_circle(500, 200, 50, GREEN, DARKGREEN, 5, 0);
        bdr_draw_circle(620, 200, 50, LIME, GREEN, 1, 0);

        // Unos triangulos de diferentes colores [azul, rojo], uno del lado del otro.
        bdr_draw_triangle(500, 480, 600, 480, 550, 580, SKYBLUE, 0);
        bdr_draw_triangle(600, 480, 700, 480, 650, 580, BLUE, 0);

        // mostrar una textura en la pantalla
        bdr_draw_texture(gobu_texture, (buder_rect_t){0, 0, 0, 0}, (buder_rect_t){380, 250, 0, 0},
                           (buder_vec2_t){1, 1}, (buder_vec2_t){gobu_texture.width / 2, gobu_texture.height / 2}, angle, WHITE, 0);
    }
    bdr_camera_end();

    // Unos textos de diferentes colores y tamaños
    bdr_draw_text_pro(fontDefault, "Gobu2d", 120, 40, 20, (buder_vec2_t){0.0f, 0.0f}, RED, 0);
    bdr_draw_text("BuderLibs", 120, 70, 40, GREEN, 0);

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    bdr_free_texture(gobu_texture);
    bdr_free_font(fontDefault);
}
