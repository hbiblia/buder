// ---------------------------------------------------------
// Buder es una librería simple de renderizado 2D
// basada en sokol_gl.h
// ---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SOKOL_IMPL
#include "buder.h"

#define SOKOL_LOG_IMPL
#include "libraries/sokol/sokol_log.h"

#define SOKOL_FETCH_IMPL
#include "libraries/sokol/sokol_fetch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libraries/stb/stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "libraries/miniaudio/miniaudio.h"

#define FONTSTASH_IMPLEMENTATION
#if defined(_MSC_VER)
#pragma warning(disable : 4996) // strncpy use in fontstash.h
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "libraries/fontstash/fontstash.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
#define SOKOL_FONTSTASH_IMPL
#include "libraries/sokol/sokol_fontstash.h"

static ma_engine audio_engine; // experimental
static FONScontext *font_ctx;  // experimental
static buder_font_t fontDefault;  // experimental

void buder_init(buder_t *buder)
{
    sg_setup(&(sg_desc){
        .logger.func = slog_func,
    });

    sgl_setup(&(sgl_desc_t){
        .logger.func = slog_func,
    });

    // sfetch_setup(&(sfetch_desc_t){
    //     .num_channels = 1,
    //     .num_lanes = 1,
    //     .logger.func = slog_func,
    // });

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

    // const int atlas_dim = round_pow2(512.0f * sapp_dpi_scale());
    font_ctx = sfons_create(&(sfons_desc_t){.width = 512, .height = 512});

    // default font
    fontDefault = buder_load_font("resources/PixelifySans-Regular.ttf");

    srand(time(0));

    buder->layers = 2; // default layers
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
    float ratio = buder->width / (float)buder->height;

    sgl_defaults();
    sgl_load_pipeline(buder->pipeline);
    sgl_viewport(0, 0, buder->width, buder->height, true);
    sgl_matrix_mode_projection();
    sgl_load_identity();
    sgl_push_matrix();
    sgl_ortho(0.0f, buder->width, buder->height, 0.0f, -1.0f, 1.0f);
}

void buder_end_frame(buder_t *buder)
{
    sgl_pop_matrix();

    sfons_flush(font_ctx);
    sg_begin_pass(&(sg_pass){.action = buder->pass_action, .swapchain = {.width = buder->width, .height = buder->height}});
    for (int i = 0; i < buder->layers; i++)
        sgl_draw_layer(i);
    // sgl_draw();
    sg_end_pass();
    sg_commit();
}

void buder_shutdown(buder_t *buder)
{
    ma_engine_uninit(&audio_engine);
    sfons_destroy(font_ctx);
    sgl_shutdown();
    sg_shutdown();
}

// ----------
// load texture, font, audio, etc
// ----------
buder_texture_t buder_load_texture(const char *path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        printf("Failed to load texture: %s", path);
        return (buder_texture_t){0};
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

    return (buder_texture_t){.id = img_id.id, .width = width, .height = height, .sampler = sampler_id.id};
}

void buder_free_texture(buder_texture_t texture)
{
    sg_destroy_image((sg_image){texture.id});
    sg_destroy_sampler((sg_sampler){texture.sampler});
}

buder_font_t buder_load_font(const char *filename)
{
    buder_font_t font_data = {0};
    font_data.font = fonsAddFont(font_ctx, buder_file_name_without_ext(filename), filename);
    return font_data;
}

void buder_free_font(buder_font_t font)
{
    // fonsDeleteFont(font_ctx, font.font);
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
// camera 2d: offset, target, rotation, zoom
// ----------
void buder_begin_camera(buder_camera_t camera)
{
    sgl_push_matrix();
    sgl_translate(camera.offset.x, camera.offset.y, 0.0f);
    sgl_rotate(sgl_rad(camera.rotation), 0.0f, 0.0f, 1.0f);
    sgl_scale(camera.zoom, camera.zoom, 1.0f);
    sgl_translate(-camera.target.x, -camera.target.y, 0.0f);
}

void buder_end_camera(void)
{
    sgl_pop_matrix();
}

// ----------
// draw
// ----------
void buder_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index)
{
    // Dibujar fill si no es completamente transparente
    if (fill_color.a > 0) {
        sgl_layer(layer_index);
        sgl_begin_quads();
        sgl_c4b(fill_color.r, fill_color.g, fill_color.b, fill_color.a);
        sgl_v2f(x, y);
        sgl_v2f(x + w, y);
        sgl_v2f(x + w, y + h);
        sgl_v2f(x, y + h);
        sgl_end();
    }

    // Línea superior
    if (outline_thickness > 0) {
        float spacing_clip = outline_thickness / 2.0f;
        buder_draw_line(x-spacing_clip, y, x + w + spacing_clip, y, outline_thickness, outline_color, layer_index);
        buder_draw_line(x + w, y, x + w, y + h, outline_thickness, outline_color, layer_index);
        buder_draw_line(x + w + spacing_clip, y + h, x - spacing_clip, y + h, outline_thickness, outline_color, layer_index);
        buder_draw_line(x, y + h, x, y, outline_thickness, outline_color, layer_index);
    }
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

void buder_draw_texture(buder_texture_t texture, buder_rect_t src, buder_rect_t dst, buder_vec2_t scale, buder_vec2_t origin, float angle, buder_color_t tint, int layer_index)
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
    sgl_c4b(tint.r, tint.g, tint.b, tint.a);
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

void buder_draw_grid(int width, int height, int cell_size, buder_color_t color, int layer_index)
{
    sgl_layer(layer_index);
    sgl_begin_lines();
    sgl_c4b(color.r, color.g, color.b, color.a);

    for (int x = 0; x <= width; x += cell_size)
    {
        sgl_v2f((float)x, 0.0f);
        sgl_v2f((float)x, (float)height);
    }

    for (int y = 0; y <= height; y += cell_size)
    {
        sgl_v2f(0.0f, (float)y);
        sgl_v2f((float)width, (float)y);
    }

    sgl_end();
}

// ----------
// text
// ----------
void buder_draw_text(buder_font_t font, const char *text, float x, float y, float font_size, buder_vec2_t origin, buder_color_t color, int layer_index)
{
    fonsClearState(font_ctx);
    fonsSetFont(font_ctx, font.font);
    fonsSetSize(font_ctx, font_size);
    fonsSetColor(font_ctx, sfons_rgba(color.r, color.g, color.b, color.a));
    int width = fonsTextBounds(font_ctx, 0, 0, text, 0, NULL);

    sgl_layer(layer_index);
    sgl_push_matrix();

    sgl_translate(x, y, 0.0f);
    // sgl_scale(scale.x, scale.y, 1.0f);
    sgl_rotate(0.0f, 0.0f, 0.0f, 1.0f);
    sgl_translate(-(width * origin.x), -origin.y, 0.0f);

    fonsDrawText(font_ctx, 0, 0, text, 0);

    sgl_pop_matrix();
}

void buder_draw_textd(const char *text, float x, float y, float font_size, buder_color_t color, int layer_index)
{
    buder_draw_text(fontDefault, text, x, y, font_size, (buder_vec2_t){0.0f, 0.0f}, color,layer_index);
}

int buder_text_measure(buder_font_t font, const char *text, float font_size)
{
    fonsClearState(font_ctx);
    fonsSetFont(font_ctx, font.font);
    fonsSetSize(font_ctx, font_size);
    return fonsTextBounds(font_ctx, 0, 0, text, 0, NULL);
}

// ----------
// audio
// ----------
void buder_play_sound(const char *audio)
{
    ma_engine_play_sound(&audio_engine, audio, NULL);
}

// ----------
// color
// ----------
buder_color_t buder_color_fade(buder_color_t color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    return (buder_color_t){color.r, color.g, color.b, (uint8_t)(color.a * alpha)};
}

// ----------
// utilities
// ----------
float buder_math_clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

int buder_random_int(int min, int max)
{
    return min + rand() % (max - min + 1);
}

char *buder_string_format(const char *str, ...)
{
    if (str == NULL)
    {
        return NULL;
    }

    va_list args;
    va_start(args, str);

    int len = vsnprintf(NULL, 0, str, args);
    va_end(args);

    if (len < 0)
    {
        return NULL;
    }

    char *buffer = (char *)malloc((size_t)len + 1);
    if (buffer == NULL)
    {
        return NULL;
    }

    va_start(args, str);
    int written = vsnprintf(buffer, (size_t)len + 1, str, args);
    va_end(args);

    if (written < 0)
    {
        free(buffer);
        return NULL;
    }

    return buffer;
}

static const char *buder_string_strpbrk(const char *s, const char *charset)
{
    const char *latestMatch = NULL;
    const char *current;

    if (s == NULL || charset == NULL)
        return NULL;

    while ((current = strpbrk(s, charset)) != NULL)
    {
        latestMatch = current;
        s = current + 1;
    }

    return latestMatch;
}

const char *buder_file_name_get(const char *filePath)
{
    const char *fileName;

    if (filePath == NULL)
        return NULL;

    fileName = buder_string_strpbrk(filePath, "\\/");

    return (fileName == NULL) ? filePath : fileName + 1;
}

char *buder_file_name_without_ext(const char *filepath)
{
    const char *fileName;
    char *result = NULL;
    const char *dotPos;

    if (filepath == NULL)
        return NULL;

    // Get the filename part
    fileName = buder_file_name_get(filepath);
    if (fileName == NULL)
        return NULL;

    // Allocate memory for the result
    result = strdup(fileName);
    if (result == NULL)
        return NULL;

    // Find last occurrence of '.'
    dotPos = strrchr(result, '.');
    if (dotPos != NULL)
    {
        // Truncate at the dot
        *((char *)dotPos) = '\0';
    }

    return result;
}

// ----------
// inputs
// ----------

static buder_vec2_t buder_mouse_position = {0.0f, 0.0f};
static bool buder_mouse_buttons[BUDER_MOUSEBUTTON_INVALID] = {0};

void buder_event_pool(buder_t *buder, const buder_event_t *event)
{
}

buder_vec2_t buder_get_mouse_position(void)
{
    return buder_mouse_position;
}

void buder_set_mouse_position(float x, float y)
{
    buder_mouse_position = (buder_vec2_t){x, y};
}

bool buder_mouse_is_button_down(buder_mousebutton button)
{
    return buder_mouse_buttons[button];
}

void buder_mouse_button_events(int mouse_button, int action)
{
    if (mouse_button == BUDER_MOUSEBUTTON_LEFT || 
        mouse_button == BUDER_MOUSEBUTTON_RIGHT || 
        mouse_button == BUDER_MOUSEBUTTON_MIDDLE) {
        
        switch (action) {
            case BUDER_EVENT_MOUSE_DOWN:
                buder_mouse_buttons[mouse_button] = true;
                break;
            
            case BUDER_EVENT_MOUSE_UP:
                buder_mouse_buttons[mouse_button] = false;
                break;
            
            default:
                break;
        }
    }
}
