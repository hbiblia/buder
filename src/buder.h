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

// Some Basic Colors
#define LIGHTGRAY   (buder_color_t){ 211, 211, 211, 255 } 
#define GRAY        (buder_color_t){ 150, 150, 150, 255 } 
#define DARKGRAY    (buder_color_t){ 90, 90, 90, 255 } 

#define YELLOW      (buder_color_t){ 255, 245, 60, 255 } 
#define GOLD        (buder_color_t){ 255, 200, 30, 255 } 
#define ORANGE      (buder_color_t){ 255, 140, 20, 255 }  

#define PINK        (buder_color_t){ 255, 120, 200, 255 }   
#define RED         (buder_color_t){ 230, 50, 60, 255 }     
#define MAROON      (buder_color_t){ 175, 40, 60, 255 }    

#define GREEN       (buder_color_t){ 0, 220, 70, 255 }     
#define LIME        (buder_color_t){ 50, 180, 50, 255 }     
#define DARKGREEN   (buder_color_t){ 0, 100, 40, 255 }     

#define SKYBLUE     (buder_color_t){ 120, 200, 255, 255 }  
#define BLUE        (buder_color_t){ 0, 130, 245, 255 }    
#define DARKBLUE    (buder_color_t){ 0, 90, 170, 255 }  

#define PURPLE      (buder_color_t){ 190, 130, 255, 255 } 
#define VIOLET      (buder_color_t){ 140, 70, 200, 255 }  
#define DARKPURPLE  (buder_color_t){ 115, 40, 140, 255 } 

#define BEIGE       (buder_color_t){ 225, 200, 150, 255 } 
#define BROWN       (buder_color_t){ 130, 100, 75, 255 }
#define DARKBROWN   (buder_color_t){ 85, 65, 50, 255 } 

#define WHITE       (buder_color_t){ 255, 255, 255, 255 } 
#define BLACK       (buder_color_t){ 0, 0, 0, 255 } 
#define BLANK       (buder_color_t){ 0, 0, 0, 0 }  

#define MAGENTA     (buder_color_t){ 255, 0, 255, 255 } 
#define BUDEWHITE   (buder_color_t){ 240, 240, 240, 255 }

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
