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

typedef struct
{
    sg_pass_action pass_action;
    sgl_pipeline pipeline;
    int width;
    int height;
    int layers;
} buder_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} buder_color_t;

typedef struct
{
    int texture_id;
    int sw;
    int sh;
} buder_texture_t;

void buder_init(buder_t *buder);
void buder_set_size(buder_t *buder, int width, int height);
void buder_begin_frame(buder_t *buder);
void buder_end_frame(buder_t *buder);
void buder_shutdown(buder_t *buder);

void buder_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void buder_draw_line(float x0, float y0, float x1, float y1, float thickness, buder_color_t color, int layer_index);
void buder_draw_circle(float x, float y, float radius, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void buder_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, buder_color_t color, int layer_index);
void buder_draw_text(const char *text, float x, float y, buder_color_t color, int layer_index);
void buder_draw_texture(buder_texture_t texture, float x, float y, float dw, float dh, int layer_index);

#endif // BUDER_H
