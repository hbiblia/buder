// ---------------------------------------------------------
// Buder es una librería simple de renderizado 2D
// basada en sokol_gl.h
// ---------------------------------------------------------
#ifndef BUDER_H
#define BUDER_H

#include "libraries/sokol/sokol_gfx.h"
#include "libraries/sokol/sokol_log.h"
// #include "libraries/sokol/sokol_fetch.h"
#include "libraries/sokol/sokol_gl.h"

typedef struct buder_t
{
    sg_pass_action pass_action;
    sgl_pipeline pipeline;
    int width;
    int height;
    uint8_t layers;
} buder_t;

typedef struct buder_color_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} buder_color_t;

typedef struct buder_rect_t
{
    float x;
    float y;
    float w;
    float h;
} buder_rect_t;

typedef struct buder_vec2_t
{
    float x;
    float y;
} buder_vec2_t;

typedef struct buder_image_t
{
    uint32_t id;
    uint32_t sampler;
    int width;
    int height;
} buder_image_t;

void buder_init(buder_t *buder);
void buder_set_stage_size(buder_t *buder, int width, int height);
void buder_set_stage_color(buder_t *buder, buder_color_t color);
void buder_begin_frame(buder_t *buder);
void buder_end_frame(buder_t *buder);
void buder_shutdown(buder_t *buder);

void buder_translate(float x, float y);
void buder_rotate(float angle);
void buder_scale(float x, float y);
void buder_begin_transform();
void buder_end_transform();

buder_image_t buder_load_texture(const char *path);
void buder_texture_free(buder_image_t texture);

void buder_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void buder_draw_line(float x0, float y0, float x1, float y1, float thickness, buder_color_t color, int layer_index);
void buder_draw_circle(float x, float y, float radius, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void buder_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, buder_color_t color, int layer_index);
void buder_draw_text(const char *text, float x, float y, buder_color_t color, int layer_index);
void buder_draw_texture(buder_image_t texture, buder_rect_t src, buder_rect_t dst, buder_vec2_t scale, buder_vec2_t origin, float angle, int layer_index);

#endif // BUDER_H
