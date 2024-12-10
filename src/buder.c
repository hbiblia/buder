// ---------------------------------------------------------
// Buder es una librería simple de renderizado 2D
// basada en sokol_gfx.h
// ---------------------------------------------------------
#define SOKOL_IMPL
#include "buder.h"

void buder_init(buder_t *buder)
{
    sg_setup(&(sg_desc){
        .logger.func = slog_func,
    });

    sgl_setup(&(sgl_desc_t){
        .logger.func = slog_func,
    });

    buder->pass_action = (sg_pass_action){
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
        },
    };

    buder->pipeline = sgl_make_pipeline(&(sg_pipeline_desc){
        .depth.write_enabled = false,
        .colors[0] = {
            .blend = {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            },
        },
    });

    buder->layers = 1; // default layers
}

void buder_set_size(buder_t *buder, int width, int height)
{
    buder->width = width;
    buder->height = height;
}

void buder_begin_frame(buder_t *buder)
{
    sgl_defaults();
    sgl_viewport(0, 0, buder->width, buder->height, true);
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, (float)buder->width, (float)buder->height, 0.0f, -1.0f, 1.0f);
    sgl_matrix_mode_modelview();
    sgl_push_matrix();
}

void buder_end_frame(buder_t *buder)
{
    sgl_pop_matrix();
    sg_begin_pass(&(sg_pass){.action = buder->pass_action, .swapchain = {.width = buder->width, .height = buder->height}});
    for (int i = 0; i < buder->layers; i++)
    {
        sgl_draw_layer(i);
    }
    sgl_draw_layer(buder->layers);
    sg_end_pass();
    sg_commit();
}

void buder_shutdown(buder_t *buder)
{
    sgl_shutdown();
    sg_shutdown();
}

void buder_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index)
{
    sgl_layer(layer_index);
    sgl_begin_quads();

    // If outline_thickness is 0 or negative, just draw a solid filled rectangle
    if (outline_thickness <= 0)
    {
        sgl_c4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        sgl_v2f(x, y);
        sgl_v2f(x + w, y);
        sgl_v2f(x + w, y + h);
        sgl_v2f(x, y + h);
        sgl_end();
        return;
    }

    // Draw outer rectangle (outline)
    sgl_c4b(outline_color.r, outline_color.g, outline_color.b, outline_color.a);
    sgl_v2f(x, y);
    sgl_v2f(x + w, y);
    sgl_v2f(x + w, y + h);
    sgl_v2f(x, y + h);

    // Draw inner rectangle (fill)
    sgl_c4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
    sgl_v2f(x + outline_thickness, y + outline_thickness);
    sgl_v2f(x + w - outline_thickness, y + outline_thickness);
    sgl_v2f(x + w - outline_thickness, y + h - outline_thickness);
    sgl_v2f(x + outline_thickness, y + h - outline_thickness);

    sgl_end();
}

void buder_draw_line(float x0, float y0, float x1, float y1, float thickness, buder_color_t color, int layer_index)
{
    // Calculate perpendicular vector
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    // Normalize and rotate 90 degrees
    float nx = -dy / length * (thickness / 2.0f);
    float ny = dx / length * (thickness / 2.0f);

    sgl_layer(layer_index);
    sgl_begin_triangles();
    sgl_c4b(color.r, color.g, color.b, color.a);

    // Define the four corners of the thick line
    sgl_v2f(x0 + nx, y0 + ny); // Top left
    sgl_v2f(x0 - nx, y0 - ny); // Bottom left
    sgl_v2f(x1 + nx, y1 + ny); // Top right

    sgl_v2f(x0 - nx, y0 - ny); // Bottom left
    sgl_v2f(x1 - nx, y1 - ny); // Bottom right
    sgl_v2f(x1 + nx, y1 + ny); // Top right

    sgl_end();
}

void buder_draw_circle(float x, float y, float radius, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index)
{
    sgl_layer(layer_index);

    // If outline_thickness is 0 or negative, draw a solid filled circle
    if (outline_thickness <= 0)
    {
        sgl_begin_triangles();
        sgl_c4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);

        for (int i = 0; i < 64; i++)
        {
            float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
            float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

            sgl_v2f(x, y);
            sgl_v2f(x + cosf(a0) * radius, y + sinf(a0) * radius);
            sgl_v2f(x + cosf(a1) * radius, y + sinf(a1) * radius);
        }
        sgl_end();
        return;
    }

    // Draw filled circle
    sgl_begin_triangles();
    sgl_c4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);

    for (int i = 0; i < 64; i++)
    {
        float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
        float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

        sgl_v2f(x, y);
        sgl_v2f(x + cosf(a0) * (radius - outline_thickness), y + sinf(a0) * (radius - outline_thickness));
        sgl_v2f(x + cosf(a1) * (radius - outline_thickness), y + sinf(a1) * (radius - outline_thickness));
    }
    sgl_end();

    // Draw outline
    sgl_begin_triangles();
    sgl_c4b(outline_color.r, outline_color.g, outline_color.b, outline_color.a);

    for (int i = 0; i < 64; i++)
    {
        float a0 = (float)i * (M_PI * 2.0f) / 64.0f;
        float a1 = (float)(i + 1) * (M_PI * 2.0f) / 64.0f;

        // Outer circle points
        float x0_outer = x + cosf(a0) * radius;
        float y0_outer = y + sinf(a0) * radius;
        float x1_outer = x + cosf(a1) * radius;
        float y1_outer = y + sinf(a1) * radius;

        // Inner circle points
        float x0_inner = x + cosf(a0) * (radius - outline_thickness);
        float y0_inner = y + sinf(a0) * (radius - outline_thickness);
        float x1_inner = x + cosf(a1) * (radius - outline_thickness);
        float y1_inner = y + sinf(a1) * (radius - outline_thickness);

        // Draw outline triangles
        sgl_v2f(x0_outer, y0_outer);
        sgl_v2f(x0_inner, y0_inner);
        sgl_v2f(x1_outer, y1_outer);

        sgl_v2f(x1_outer, y1_outer);
        sgl_v2f(x0_inner, y0_inner);
        sgl_v2f(x1_inner, y1_inner);
    }
    sgl_end();
}

void buder_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, buder_color_t color, int layer_index)
{
    sgl_layer(layer_index);
    sgl_begin_triangles();
    sgl_c4b(color.r, color.g, color.b, color.a);
    sgl_v2f(x0, y0);
    sgl_v2f(x1, y1);
    sgl_v2f(x2, y2);
    sgl_end();
}

void buder_draw_text(const char *text, float x, float y, buder_color_t color, int layer_index)
{
    // TODO: Implement text rendering
}

void buder_draw_texture(buder_texture_t texture, float x, float y, float dw, float dh, int layer_index)
{
    // TODO: Implement texture rendering
}

