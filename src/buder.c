// ---------------------------------------------------------
// Buder es una librería simple de renderizado 2D
// basada en sokol_gl.h
// ---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SOKOL_IMPL
#include "buder.h"

#define SOKOL_LOG_IMPL
#include "libraries/sokol/sokol_log.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "libraries/sokol/sokol_debugtext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libraries/stb/stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "libraries/miniaudio/miniaudio.h"

#define NUM_FONTS (3)
#define FONT_KC854 (0)
#define FONT_C64 (1)
#define FONT_ORIC (2)

static ma_engine audio_engine; // experimental

void buder_init(buder_t *buder)
{
    sg_setup(&(sg_desc){
        .logger.func = slog_func,
    });

    sgl_setup(&(sgl_desc_t){
        .logger.func = slog_func,
    });

    // temporal fonts
    sdtx_setup(&(sdtx_desc_t){
        .fonts = {
            [FONT_KC854] = sdtx_font_kc854(),
            [FONT_C64] = sdtx_font_c64(),
            [FONT_ORIC] = sdtx_font_oric()},
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

    ma_result result = ma_engine_init(NULL, &audio_engine);
    if (result != MA_SUCCESS)
    {
        printf("Failed to initialize audio engine\n");
    }

    srand(time(0));

    buder->layers = 1; // default layers
    buder->width = 800;
    buder->height = 600;
}

void buder_set_stage_size(buder_t *buder, int width, int height)
{
    buder->width = width;
    buder->height = height;
}

void buder_set_stage_color(buder_t *buder, buder_color_t color)
{
    buder->pass_action.colors[0].clear_value = (sg_color){color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
}

void buder_begin_frame(buder_t *buder)
{
    sgl_defaults();
    sgl_load_pipeline(buder->pipeline);
    sdtx_canvas(buder->width, buder->height);

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
    sdtx_draw();
    sg_end_pass();
    sg_commit();
}

void buder_shutdown(buder_t *buder)
{
    ma_engine_uninit(&audio_engine);
    sgl_shutdown();
    sg_shutdown();
}

// ----------
// load texture, font, audio, etc
// ----------
buder_image_t buder_load_texture(const char *path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        printf("Failed to load texture: %s", path);
        return (buder_image_t){0};
    }

    sg_image img_id = sg_make_image(&(sg_image_desc){
        .width = width,
        .height = height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.subimage[0][0] = {
            .ptr = data,
            .size = (size_t)(width * height * 4),
        },
    });

    sg_sampler sampler_id = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT,
    });

    stbi_image_free(data);

    return (buder_image_t){.id = img_id.id, .width = width, .height = height, .sampler = sampler_id.id};
}

void buder_texture_free(buder_image_t texture)
{
    sg_destroy_image((sg_image){texture.id});
    sg_destroy_sampler((sg_sampler){texture.sampler});
}

// ----------
// trasnform: translate, rotate, scale
// ----------

void buder_translate(float x, float y)
{
    sgl_translate(x, y, 0.0f);
}

void buder_rotate(float angle)
{
    sgl_rotate(angle, 0.0f, 0.0f, 1.0f);
}

void buder_scale(float x, float y)
{
    sgl_scale(x, y, 1.0f);
}

void buder_begin_transform()
{
    sgl_push_matrix();
}

void buder_end_transform()
{
    sgl_pop_matrix();
}

// ----------
// draw
// ----------
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
    // sgl_layer(layer_index);
    sdtx_font(0);
    sdtx_origin(1, 1);
    sdtx_color3b(color.r, color.g, color.b);
    sdtx_puts(text);
    // sdtx_move(x, y);
}

void buder_draw_texture(buder_image_t texture, buder_rect_t src, buder_rect_t dst, buder_vec2_t scale, buder_vec2_t origin, float angle, int layer_index)
{
    src.w = (src.w == 0) ? texture.width : src.w;
    src.h = (src.h == 0) ? texture.height : src.h;

    dst.w = (dst.w == 0) ? src.w : dst.w;
    dst.h = (dst.h == 0) ? src.h : dst.h;

    sgl_layer(layer_index);

    sgl_enable_texture();
    sgl_texture(
        (sg_image){texture.id},
        (sg_sampler){texture.sampler});

    // sgl_load_default_pipeline();

    float tex_left = src.x / (float)texture.width;
    float tex_top = src.y / (float)texture.height;
    float tex_right = (src.x + src.w) / (float)texture.width;
    float tex_bottom = (src.y + src.h) / (float)texture.height;

    sgl_push_matrix();

    sgl_translate(dst.x, dst.y, 0.0f);
    sgl_scale(scale.x, scale.y, 1.0f);
    sgl_rotate(angle, 0.0f, 0.0f, 1.0f);
    sgl_translate(-origin.x, -origin.y, 0.0f);

    sgl_begin_quads();
    sgl_c4f(1.0f, 1.0f, 1.0f, 1.0f);
    {
        sgl_v2f_t2f(0.0f, 0.0f, tex_left, tex_top);
        sgl_v2f_t2f(0.0f, dst.h, tex_left, tex_bottom);
        sgl_v2f_t2f(dst.w, dst.h, tex_right, tex_bottom);
        sgl_v2f_t2f(dst.w, 0.0f, tex_right, tex_top);
    }
    sgl_end();

    sgl_pop_matrix();

    sgl_disable_texture();
}

// ----------
// audio
// ----------

void buder_play_sound(const char *audio)
{
    ma_engine_play_sound(&audio_engine, audio, NULL);
}

// ----------
// utilities
// ----------

int buder_random_int(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// ----------
// inputs
// ----------

void buder_event_pool(buder_t *buder, const buder_event_t *event)
{
}
