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

static ma_engine audio_engine;   // experimental
static FONScontext *font_ctx;    // experimental
static buder_font_t fontDefault; // experimental

static void bdr_event_pool_end(buder_t *buder);

void buder_init(buder_t *buder, int width, int height)
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
    fontDefault = bdr_load_font("resources/Silkscreen-Regular.ttf");

    srand(time(0));

    buder->layers = 1;
    bdr_viewport_size(buder, width, height);
    bdr_viewport_init(buder);
}

void buder_shutdown(buder_t *buder)
{
    ma_engine_uninit(&audio_engine);
    sfons_destroy(font_ctx);
    sgl_shutdown();
    sg_shutdown();
}

void bdr_viewport_size(buder_t *buder, int width, int height)
{
    buder->width = width;
    buder->height = height;
    bdr_viewport_init(buder);
}

void bdr_viewport_color(buder_t *buder, buder_color_t color)
{
    buder->pass_action.colors[0].clear_value = (sg_color){color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
}

void bdr_viewport_init(buder_t *buder)
{
    sgl_defaults();
    sgl_load_pipeline(buder->pipeline);
    sgl_viewport(0, 0, buder->width, buder->height, true);
    sgl_matrix_mode_projection();
    sgl_load_identity();
    sgl_ortho(0.0f, buder->width, buder->height, 0.0f, -1.0f, 1.0f);
}

void bdr_viewport_present(buder_t *buder)
{
    bdr_event_pool_end(buder);

    sfons_flush(font_ctx);
    sg_begin_pass(&(sg_pass){.action = buder->pass_action, .swapchain = {.width = buder->width, .height = buder->height}});
    for (int i = 0; i < buder->layers; i++)
        sgl_draw_layer(i);
    sg_end_pass();
    sg_commit();
}

void bdr_transform_begin()
{
    sgl_push_matrix();
}

void bdr_position(float x, float y)
{
    sgl_translate(x, y, 0.0f);
}

void bdr_angle(float angle)
{
    sgl_rotate(angle, 0.0f, 0.0f, 1.0f);
}

void bdr_scale(float x, float y)
{
    sgl_scale(x, y, 1.0f);
}

void bdr_transform_end()
{
    sgl_pop_matrix();
}

// MODULE CAMERA
void bdr_camera_begin(buder_camera_t camera)
{
    sgl_push_matrix();
    sgl_translate(camera.offset.x, camera.offset.y, 0.0f);
    sgl_rotate(sgl_rad(camera.rotation), 0.0f, 0.0f, 1.0f);
    sgl_scale(camera.zoom, camera.zoom, 1.0f);
    sgl_translate(-camera.target.x, -camera.target.y, 0.0f);
}

void bdr_camera_end(void)
{
    sgl_pop_matrix();
}

// MODULO TEXTURE, SPRITES...
buder_texture_t bdr_load_texture(const char *filename)
{
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        printf("Failed to load texture: %s", filename);
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

bool bdr_texture_is_valid(buder_texture_t texture)
{
    return texture.id != 0;
}

void bdr_free_texture(buder_texture_t texture)
{
    sg_destroy_image((sg_image){texture.id});
    sg_destroy_sampler((sg_sampler){texture.sampler});
}

void bdr_draw_texture_pro(buder_texture_t texture, buder_rect_t src, buder_rect_t dst, buder_vec2_t scale, buder_vec2_t origin, float angle, buder_color_t tint, int layer_index)
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

void bdr_draw_texture_rect(buder_texture_t texture, buder_rect_t src, buder_vec2_t position, buder_color_t tint, int layer_index)
{
    buder_rect_t dst = {position.x, position.y, fabsf(src.w), fabsf(src.h)};
    buder_vec2_t scale = {1.0f, 1.0f};
    buder_vec2_t origin = {0.0f, 0.0f};
    bdr_draw_texture_pro(texture, src, dst, scale, origin, 0.0f, tint, layer_index);
}

void bdr_draw_texture(buder_texture_t texture, buder_vec2_t position, buder_color_t tint, int layer_index)
{
    buder_rect_t src = {0.0f, 0.0f, texture.width, texture.height};
    bdr_draw_texture_rect(texture, src, position, tint, layer_index);
}

buder_rect_t bdr_sprite_animation_update(buder_sprite_animation_t *animation)
{
    buder_sprite_animation_frame_t *currentFrame = &animation->frames[animation->active];

    if (animation->playing)
    {
        animation->frames_count++;
        if (animation->frames_count >= (60 / animation->frames_speed)) // animation->target_fps / speed
        {
            animation->frames_count = 0;
            currentFrame->frame++;
            if (currentFrame->frame > currentFrame->end)
            {
                currentFrame->frame = currentFrame->loop ? currentFrame->start : currentFrame->end;
            }
        }
    }

    if (currentFrame->hframe == 0 || currentFrame->vframe == 0) {
        return (buder_rect_t){0};
    }

    int twidth = currentFrame->texture.width;
    int theight = currentFrame->texture.height;

    int frame_w = twidth / currentFrame->hframe;
    int frame_h = theight / currentFrame->vframe;

    float frame_x = (currentFrame->frame % currentFrame->hframe) * frame_w;
    float frame_y = (currentFrame->frame / currentFrame->hframe) * frame_h;

    return (buder_rect_t){frame_x, frame_y, frame_w, frame_h};
}

buder_vec2_t bdr_sprite_animation_get_size_frame(buder_sprite_animation_t *animation)
{
    buder_sprite_animation_frame_t *currentFrame = &animation->frames[animation->active];
    return (buder_vec2_t){currentFrame->texture.width / currentFrame->hframe, currentFrame->texture.height / currentFrame->vframe};
}

buder_texture_t bdr_sprite_animation_get_texture(buder_sprite_animation_t *animation)
{
    return animation->frames[animation->active].texture;
}

// solo indica si ha llegado al último frame
bool bdr_sprite_animation_is_finished(buder_sprite_animation_t *animation)
{
    buder_sprite_animation_frame_t *currentFrame = &animation->frames[animation->active];
    return currentFrame->frame >= currentFrame->end;
}

void bdr_sprite_animation_play(buder_sprite_animation_t *animation, int active)
{
    if (bdr_texture_is_valid(animation->frames[active].texture))
    {
        animation->active = active;
        animation->frames_count = 0;
        animation->frames[animation->active].frame = animation->frames[animation->active].start;
        animation->playing = true;
    }
}

void bdr_sprite_animation_set_speed(buder_sprite_animation_t *animation, int speed)
{
    animation->frames_speed = speed;
}

void bdr_sprite_animation_set_frame(buder_sprite_animation_t *animation, int frame)
{
    animation->frames[animation->active].frame = frame;
}

// void bdr_sprite_animation_set_fps(buder_sprite_animation_t *animation, int fps)
// {
//     animation->target_fps = fps;
// }

void bdr_sprite_animation_register(buder_sprite_animation_t *animation, int key_id, buder_sprite_animation_frame_t frame)
{
    // animation->target_fps = animation->target_fps <= 0 ? 60 : animation->target_fps; // config default fps
    animation->frames_speed = animation->frames_speed <= 0 ? 12 : animation->frames_speed; // config default speed
    frame.start = frame.start < 0 ? 0 : frame.start;
    frame.end = frame.end <= 0 ? (frame.hframe * frame.vframe) - 1 : (frame.end - 1);
    animation->frames[key_id] = frame;
    animation->length++;
}

void bdr_sprite_animation_unregister(buder_sprite_animation_t *animation, int key_id)
{
    bdr_free_texture(animation->frames[key_id].texture);
    animation->frames[key_id] = (buder_sprite_animation_frame_t){0};
    animation->length--;
}

void bdr_sprite_animation_destroy(buder_sprite_animation_t *animation)
{
    for (int i = animation->length - 1; i >= 0; i--)
    {
        bdr_sprite_animation_unregister(animation, i);
    }
}

// MODULO FONTS y TEXT
buder_font_t bdr_load_font(const char *filename)
{
    buder_font_t font_data = {0};
    font_data.id = fonsAddFont(font_ctx, bdr_file_name_without_ext(filename), filename);
    return font_data;
}

void bdr_free_font(buder_font_t font)
{
    // fonsDeleteFont(font_ctx, font.id);
}

buder_font_t bdr_get_default_font(void)
{
    return fontDefault;
}

void bdr_draw_text_pro(buder_font_t font, const char *text, float x, float y, float font_size, buder_vec2_t origin, buder_color_t color, int layer_index)
{
    fonsClearState(font_ctx);
    fonsSetFont(font_ctx, font.id);
    fonsSetSize(font_ctx, font_size);
    fonsSetColor(font_ctx, bdr_color_to_uint(color));
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

void bdr_draw_text(const char *text, float x, float y, float font_size, buder_color_t color, int layer_index)
{
    bdr_draw_text_pro(fontDefault, text, x, y, font_size, (buder_vec2_t){0.0f, 0.0f}, color, layer_index);
}

int bdr_text_measure(buder_font_t font, const char *text, float font_size)
{
    fonsClearState(font_ctx);
    fonsSetFont(font_ctx, font.id);
    fonsSetSize(font_ctx, font_size);
    return fonsTextBounds(font_ctx, 0, 0, text, 0, NULL);
}

// MMODULO AUDIO (experimental)
// MODULE AUDIO
void bdr_play_sound(const char *filename)
{
    ma_engine_play_sound(&audio_engine, filename, NULL);
}

// buder_sound_t bdr_load_sound(const char *filename)
// {
//     buder_sound_t sound = {0};
//     sound.id = ma_sound_init_from_file(&audio_engine, filename, 0, NULL, NULL, &sound.sound);
//     if (sound.id != MA_SUCCESS) {
//         return sound;
//     }
//     return sound;
// }

// void bdr_free_sound(buder_sound_t sound)
// {
// }

// void bdr_pause_sound(buder_sound_t sound)
// {
// }

// void bdr_resume_sound(buder_sound_t sound)
// {
// }

// void bdr_stop_sound(buder_sound_t sound)
// {
// }

// void bdr_set_sound_volume(buder_sound_t sound, float volume)
// {
// }

// void bdr_set_sound_pitch(buder_sound_t sound, float pitch)
// {
// }

// void bdr_set_sound_loop(buder_sound_t sound, bool loop)
// {
// }

// MODULE SHAPE
void bdr_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index)
{
    // Dibujar fill si no es completamente transparente
    if (fill_color.a > 0)
    {
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
    if (outline_thickness > 0)
    {
        float spacing_clip = outline_thickness / 2.0f;
        bdr_draw_line(x - spacing_clip, y, x + w + spacing_clip, y, outline_thickness, outline_color, layer_index);
        bdr_draw_line(x + w, y, x + w, y + h, outline_thickness, outline_color, layer_index);
        bdr_draw_line(x + w + spacing_clip, y + h, x - spacing_clip, y + h, outline_thickness, outline_color, layer_index);
        bdr_draw_line(x, y + h, x, y, outline_thickness, outline_color, layer_index);
    }
}

void bdr_draw_line(float x0, float y0, float x1, float y1, float thickness, buder_color_t color, int layer_index)
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

void bdr_draw_circle(float x, float y, float radius, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index)
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

void bdr_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, buder_color_t color, int layer_index)
{
    sgl_layer(layer_index);
    sgl_begin_triangles();
    sgl_c4b(color.r, color.g, color.b, color.a);
    sgl_v2f(x0, y0);
    sgl_v2f(x1, y1);
    sgl_v2f(x2, y2);
    sgl_end();
}

void bdr_draw_grid(int width, int height, int cell_size, buder_color_t color, int layer_index)
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

// MODULE COLOR
buder_color_t bdr_color_fade(buder_color_t color, float alpha)
{
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    return (buder_color_t){color.r, color.g, color.b, (uint8_t)(color.a * alpha)};
}

uint32_t bdr_color_to_uint(buder_color_t color)
{
    return sfons_rgba(color.r, color.g, color.b, color.a);
}

// MODULE MATH
float bdr_math_clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

int bdr_math_random_int(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// MODULE STRING
char *bdr_string_format(const char *str, ...)
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

const char *bdr_string_strpbrk(const char *s, const char *charset)
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

// MODULE PATH, FILE
const char *bdr_file_name_get(const char *filePath)
{
    const char *fileName;

    if (filePath == NULL)
        return NULL;

    fileName = bdr_string_strpbrk(filePath, "\\/");

    return (fileName == NULL) ? filePath : fileName + 1;
}

char *bdr_file_name_without_ext(const char *filepath)
{
    const char *fileName;
    char *result = NULL;
    const char *dotPos;

    if (filepath == NULL)
        return NULL;

    // Get the filename part
    fileName = bdr_file_name_get(filepath);
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

// MODULE INPUT AND EVENTS
static buder_vec2_t buder_mouse_position = {0.0f, 0.0f};
static bool buder_mouse_buttons[BUDER_MOUSEBUTTON_INVALID] = {0};
static bool buder_keys[BUDER_KEY_MENU] = {0};

void bdr_event_pool(buder_t *buder, const buder_event_t *event)
{
    bdr_event_keyboard_events(buder, event->key, event->type);
    bdr_event_mouse_events(buder, event->mouse_button, event->type);
    bdr_event_mouse_position(buder, event->mouse_x, event->mouse_y);
    bdr_event_mouse_wheel(buder, event->scroll_x, event->scroll_y);

    if (event->type == BUDER_EVENT_RESIZED)
    {
        bdr_viewport_size(buder, event->width, event->height);
    }
}

static void bdr_event_pool_end(buder_t *buder)
{
    // Update previous mouse wheel
    buder->input._previous_mouse_wheel = buder->input._current_mouse_wheel;
    buder->input._current_mouse_wheel = (buder_vec2_t){0.0f, 0.0f};

    // Update previous mouse position
    buder->input._previous_mouse_position = buder->input._current_mouse_position;

    // keyboard previous state
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
    {
        buder->input._previous_key_state[i] = buder->input._current_key_state[i];
    }

    // mouse previous state
    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++)
    {
        buder->input._previous_mouse_state[i] = buder->input._current_mouse_state[i];
    }
}

void bdr_event_mouse_position(buder_t *buder, float x, float y)
{
    buder->input._current_mouse_position = (buder_vec2_t){x, y};
}

void bdr_event_mouse_wheel(buder_t *buder, float wheel_x, float wheel_y)
{
    buder->input._current_mouse_wheel = (buder_vec2_t){wheel_x, wheel_y};
}

void bdr_event_keyboard_events(buder_t *buder, int key, int action)
{
    switch (action)
    {
    case BUDER_EVENT_KEY_DOWN:
        buder->input._current_key_state[key] = true;
        break;
    case BUDER_EVENT_KEY_UP:
        buder->input._current_key_state[key] = false;
        break;
    default:
        break;
    }
}

void bdr_event_mouse_events(buder_t *buder, int mouse_button, int action)
{
    if (mouse_button == BUDER_MOUSEBUTTON_LEFT ||
        mouse_button == BUDER_MOUSEBUTTON_RIGHT ||
        mouse_button == BUDER_MOUSEBUTTON_MIDDLE)
    {
        switch (action)
        {
        case BUDER_EVENT_MOUSE_DOWN:
            buder->input._current_mouse_state[mouse_button] = true;
            break;

        case BUDER_EVENT_MOUSE_UP:
            buder->input._current_mouse_state[mouse_button] = false;
            break;
        default:
            break;
        }
    }
}

int bdr_mouse_x(buder_t *buder)
{
    return (int)buder->input._current_mouse_position.x;
}

int bdr_mouse_y(buder_t *buder)
{
    return (int)buder->input._current_mouse_position.y;
}

buder_vec2_t bdr_mouse_get_pos(buder_t *buder)
{
    return buder->input._current_mouse_position;
}

bool bdr_mouse_is_down(buder_t *buder, buder_mousebutton button)
{
    return buder->input._current_mouse_state[button];
}

bool bdr_mouse_is_up(buder_t *buder, buder_mousebutton button)
{
    return !buder->input._current_mouse_state[button];
}

bool bdr_mouse_is_pressed(buder_t *buder, buder_mousebutton button)
{
    bool is_button_changed = buder->input._current_mouse_state[button] != buder->input._previous_mouse_state[button];
    bool is_button_down = buder->input._current_mouse_state[button];
    buder->input._previous_mouse_state[button] = buder->input._current_mouse_state[button];
    return is_button_changed && is_button_down;
}

bool bdr_mouse_is_released(buder_t *buder, buder_mousebutton button)
{
    bool is_button_changed = buder->input._current_mouse_state[button] != buder->input._previous_mouse_state[button];
    bool is_button_up = !buder->input._current_mouse_state[button];
    buder->input._previous_mouse_state[button] = buder->input._current_mouse_state[button];
    return is_button_changed && is_button_up;
}

float bdr_mouse_wheel_move(buder_t *buder)
{
    float result = 0.0f;

    if (fabs(buder->input._current_mouse_wheel.x) > fabs(buder->input._current_mouse_wheel.y))
        result = buder->input._current_mouse_wheel.x;
    else
        result = buder->input._current_mouse_wheel.y;

    return result;
}

bool bdr_keyboard_is_up(buder_t *buder, buder_keyboard key)
{
    return !buder->input._current_key_state[key];
}

bool bdr_keyboard_is_down(buder_t *buder, buder_keyboard key)
{
    return buder->input._current_key_state[key];
}

bool bdr_keyboard_is_pressed(buder_t *buder, buder_keyboard key)
{
    bool is_key_changed = buder->input._current_key_state[key] != buder->input._previous_key_state[key];
    bool is_key_down = buder->input._current_key_state[key];
    buder->input._previous_key_state[key] = buder->input._current_key_state[key];
    return is_key_changed && is_key_down;
}

bool bdr_keyboard_is_released(buder_t *buder, buder_keyboard key)
{
    bool is_key_changed = buder->input._current_key_state[key] != buder->input._previous_key_state[key];
    bool is_key_up = !buder->input._current_key_state[key];
    buder->input._previous_key_state[key] = buder->input._current_key_state[key];
    return is_key_changed && is_key_up;
}

// COLLISION DETECTION
bool bdr_collision_point_rec(buder_vec2_t point, buder_rect_t rec)
{
    return (point.x >= rec.x && point.x <= rec.x + rec.w && point.y >= rec.y && point.y <= rec.y + rec.h);
}

bool bdr_collision_rec_rec(buder_rect_t rec1, buder_rect_t rec2)
{
    return !(rec2.x > rec1.x + rec1.w || rec2.x + rec2.w < rec1.x || rec2.y > rec1.y + rec1.h || rec2.y + rec2.h < rec1.y);
}

bool bdr_collision_point_circle(buder_vec2_t point, buder_vec2_t center, float radius)
{
    return (sqrtf((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y)) <= radius);
}

bool bdr_collision_circle_rec(buder_vec2_t center, float radius, buder_rect_t rec)
{
    float rec_center_x = rec.x + rec.w / 2;
    float rec_center_y = rec.y + rec.h / 2;
    float dx = fabs(center.x - rec_center_x);
    float dy = fabs(center.y - rec_center_y);

    if (dx > (rec.w / 2 + radius))
        return false;
    if (dy > (rec.h / 2 + radius))
        return false;

    if (dx <= (rec.w / 2))
        return true;
    if (dy <= (rec.h / 2))
        return true;

    float corner_dist = (dx - rec.w / 2) * (dx - rec.w / 2) + (dy - rec.h / 2) * (dy - rec.h / 2);
    return (corner_dist <= (radius * radius));
}

