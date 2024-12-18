// ---------------------------------------------------------
// Buder es una librería simple de renderizado 2D
// basada en sokol_gl.h
// ---------------------------------------------------------
#ifndef BUDER_H
#define BUDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "libraries/sokol/sokol_gfx.h"
#include "libraries/sokol/sokol_gl.h"
// #include "libraries/stb/stb_ds.h"

#define MAX_KEYBOARD_KEYS 512
#define MAX_MOUSE_BUTTONS 3
#define MAX_SPRITE_ANIMATIONS 20

// Some Basic Colors
#define GRIDBLACK    (buder_color_t){ 20, 20, 20, 255 }
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

typedef enum buder_event_type
{
    BUDER_EVENT_INVALID,
    BUDER_EVENT_KEY_DOWN,
    BUDER_EVENT_KEY_UP,
    BUDER_EVENT_CHAR,
    BUDER_EVENT_MOUSE_DOWN,
    BUDER_EVENT_MOUSE_UP,
    BUDER_EVENT_MOUSE_SCROLL,
    BUDER_EVENT_MOUSE_MOVE,
    BUDER_EVENT_MOUSE_ENTER,
    BUDER_EVENT_MOUSE_LEAVE,
    BUDER_EVENT_TOUCHES_BEGAN,
    BUDER_EVENT_TOUCHES_MOVED,
    BUDER_EVENT_TOUCHES_ENDED,
    BUDER_EVENT_TOUCHES_CANCELLED,
    BUDER_EVENT_RESIZED,
    BUDER_EVENT_ICONIFIED,
    BUDER_EVENT_RESTORED,
    BUDER_EVENT_FOCUSED,
    BUDER_EVENT_UNFOCUSED,
    BUDER_EVENT_SUSPENDED,
    BUDER_EVENT_RESUMED,
    BUDER_EVENT_QUIT_REQUESTED,
    BUDER_EVENT_CLIPBOARD_PASTED,
    BUDER_EVENT_FILES_DROPPED,
    _BUDER_EVENT_NUM,
    _BUDER_EVENT_FORCE_U32 = 0x7FFFFFFF
} buder_event_type;

typedef enum buder_keyboard
{
    BUDER_KEY_INVALID = 0,
    BUDER_KEY_SPACE = 32,
    BUDER_KEY_APOSTROPHE = 39, /* ' */
    BUDER_KEY_COMMA = 44,      /* , */
    BUDER_KEY_MINUS = 45,      /* - */
    BUDER_KEY_PERIOD = 46,     /* . */
    BUDER_KEY_SLASH = 47,      /* / */
    BUDER_KEY_0 = 48,
    BUDER_KEY_1 = 49,
    BUDER_KEY_2 = 50,
    BUDER_KEY_3 = 51,
    BUDER_KEY_4 = 52,
    BUDER_KEY_5 = 53,
    BUDER_KEY_6 = 54,
    BUDER_KEY_7 = 55,
    BUDER_KEY_8 = 56,
    BUDER_KEY_9 = 57,
    BUDER_KEY_SEMICOLON = 59, /* ; */
    BUDER_KEY_EQUAL = 61,     /* = */
    BUDER_KEY_A = 65,
    BUDER_KEY_B = 66,
    BUDER_KEY_C = 67,
    BUDER_KEY_D = 68,
    BUDER_KEY_E = 69,
    BUDER_KEY_F = 70,
    BUDER_KEY_G = 71,
    BUDER_KEY_H = 72,
    BUDER_KEY_I = 73,
    BUDER_KEY_J = 74,
    BUDER_KEY_K = 75,
    BUDER_KEY_L = 76,
    BUDER_KEY_M = 77,
    BUDER_KEY_N = 78,
    BUDER_KEY_O = 79,
    BUDER_KEY_P = 80,
    BUDER_KEY_Q = 81,
    BUDER_KEY_R = 82,
    BUDER_KEY_S = 83,
    BUDER_KEY_T = 84,
    BUDER_KEY_U = 85,
    BUDER_KEY_V = 86,
    BUDER_KEY_W = 87,
    BUDER_KEY_X = 88,
    BUDER_KEY_Y = 89,
    BUDER_KEY_Z = 90,
    BUDER_KEY_LEFT_BRACKET = 91,  /* [ */
    BUDER_KEY_BACKSLASH = 92,     /* \ */
    BUDER_KEY_RIGHT_BRACKET = 93, /* ] */
    BUDER_KEY_GRAVE_ACCENT = 96,  /* ` */
    BUDER_KEY_WORLD_1 = 161,      /* non-US #1 */
    BUDER_KEY_WORLD_2 = 162,      /* non-US #2 */
    BUDER_KEY_ESCAPE = 256,
    BUDER_KEY_ENTER = 257,
    BUDER_KEY_TAB = 258,
    BUDER_KEY_BACKSPACE = 259,
    BUDER_KEY_INSERT = 260,
    BUDER_KEY_DELETE = 261,
    BUDER_KEY_RIGHT = 262,
    BUDER_KEY_LEFT = 263,
    BUDER_KEY_DOWN = 264,
    BUDER_KEY_UP = 265,
    BUDER_KEY_PAGE_UP = 266,
    BUDER_KEY_PAGE_DOWN = 267,
    BUDER_KEY_HOME = 268,
    BUDER_KEY_END = 269,
    BUDER_KEY_CAPS_LOCK = 280,
    BUDER_KEY_SCROLL_LOCK = 281,
    BUDER_KEY_NUM_LOCK = 282,
    BUDER_KEY_PRINT_SCREEN = 283,
    BUDER_KEY_PAUSE = 284,
    BUDER_KEY_F1 = 290,
    BUDER_KEY_F2 = 291,
    BUDER_KEY_F3 = 292,
    BUDER_KEY_F4 = 293,
    BUDER_KEY_F5 = 294,
    BUDER_KEY_F6 = 295,
    BUDER_KEY_F7 = 296,
    BUDER_KEY_F8 = 297,
    BUDER_KEY_F9 = 298,
    BUDER_KEY_F10 = 299,
    BUDER_KEY_F11 = 300,
    BUDER_KEY_F12 = 301,
    BUDER_KEY_F13 = 302,
    BUDER_KEY_F14 = 303,
    BUDER_KEY_F15 = 304,
    BUDER_KEY_F16 = 305,
    BUDER_KEY_F17 = 306,
    BUDER_KEY_F18 = 307,
    BUDER_KEY_F19 = 308,
    BUDER_KEY_F20 = 309,
    BUDER_KEY_F21 = 310,
    BUDER_KEY_F22 = 311,
    BUDER_KEY_F23 = 312,
    BUDER_KEY_F24 = 313,
    BUDER_KEY_F25 = 314,
    BUDER_KEY_KP_0 = 320,
    BUDER_KEY_KP_1 = 321,
    BUDER_KEY_KP_2 = 322,
    BUDER_KEY_KP_3 = 323,
    BUDER_KEY_KP_4 = 324,
    BUDER_KEY_KP_5 = 325,
    BUDER_KEY_KP_6 = 326,
    BUDER_KEY_KP_7 = 327,
    BUDER_KEY_KP_8 = 328,
    BUDER_KEY_KP_9 = 329,
    BUDER_KEY_KP_DECIMAL = 330,
    BUDER_KEY_KP_DIVIDE = 331,
    BUDER_KEY_KP_MULTIPLY = 332,
    BUDER_KEY_KP_SUBTRACT = 333,
    BUDER_KEY_KP_ADD = 334,
    BUDER_KEY_KP_ENTER = 335,
    BUDER_KEY_KP_EQUAL = 336,
    BUDER_KEY_LEFT_SHIFT = 340,
    BUDER_KEY_LEFT_CONTROL = 341,
    BUDER_KEY_LEFT_ALT = 342,
    BUDER_KEY_LEFT_SUPER = 343,
    BUDER_KEY_RIGHT_SHIFT = 344,
    BUDER_KEY_RIGHT_CONTROL = 345,
    BUDER_KEY_RIGHT_ALT = 346,
    BUDER_KEY_RIGHT_SUPER = 347,
    BUDER_KEY_MENU = 348,
} buder_keyboard;

typedef enum buder_mousebutton
{
    BUDER_MOUSEBUTTON_LEFT = 0,
    BUDER_MOUSEBUTTON_RIGHT = 1,
    BUDER_MOUSEBUTTON_MIDDLE = 2,
    BUDER_MOUSEBUTTON_INVALID = 3,
} buder_mousebutton;

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

typedef struct buder_camera_t
{
    buder_vec2_t offset;
    buder_vec2_t target;
    float rotation;
    float zoom;
} buder_camera_t;

typedef struct buder_texture_t
{
    uint32_t id;
    uint32_t sampler;
    int width;
    int height;
} buder_texture_t;

typedef struct buder_font_t
{
    int id;
} buder_font_t;

// typedef struct buder_sound_t
// {
//     uint32_t id;
// } buder_sound_t;

typedef struct buder_sprite_animation_frame_t
{
    int hframe;
    int vframe;
    int frame;
    bool loop;
    int start;
    int end;
    buder_texture_t texture;
} buder_sprite_animation_frame_t;

typedef struct buder_sprite_animation_t
{
    buder_sprite_animation_frame_t frames[MAX_SPRITE_ANIMATIONS];
    int frames_count;
    int frames_speed;
    int target_fps;
    int active;
    uint32_t length;
    bool playing;
} buder_sprite_animation_t;

typedef struct buder_event_t
{
    buder_event_type type;
    buder_keyboard key;
    uint32_t char_code;
    buder_mousebutton mouse_button;
    int width;
    int height;
    float mouse_x;
    float mouse_y;
    float mouse_dx;
    float mouse_dy;
    float scroll_x;
    float scroll_y;
} buder_event_t;

typedef struct buder_t
{
    sg_pass_action pass_action;
    sgl_pipeline pipeline;
    // struct window config
    int width;
    int height;
    uint8_t layers; // experimental
    // struct input config
    struct {
        bool mouse_buttons[BUDER_MOUSEBUTTON_INVALID];
        int _current_key_state[MAX_KEYBOARD_KEYS];
        int _previous_key_state[MAX_KEYBOARD_KEYS];
        int _current_mouse_state[MAX_MOUSE_BUTTONS];
        int _previous_mouse_state[MAX_MOUSE_BUTTONS];
        buder_vec2_t _current_mouse_wheel;
        buder_vec2_t _previous_mouse_wheel;
        buder_vec2_t _current_mouse_position;
        buder_vec2_t _previous_mouse_position;
    } input;
} buder_t;

#define BUDER_RECT_ZERO (buder_rect_t){0, 0, 0, 0}

// MODULE CORE
void buder_init(buder_t *buder, int width, int height);
void buder_shutdown(buder_t *buder);

void bdr_viewport_size(buder_t *buder, int width, int height);
void bdr_viewport_color(buder_t *buder, buder_color_t color);

void bdr_viewport_init(buder_t *buder);
void bdr_viewport_present(buder_t *buder);

void bdr_transform_begin(void);
void bdr_position(float x, float y);
void bdr_scale(float x, float y);
void bdr_angle(float angle);
void bdr_transform_end();

// MODULE CAMERA
void bdr_camera_begin(buder_camera_t camera);
void bdr_camera_end(void);

// MODULO TEXTURE AND SPRITES
buder_texture_t bdr_load_texture(const char *filename);
bool bdr_texture_is_valid(buder_texture_t texture);
void bdr_free_texture(buder_texture_t texture);
void bdr_draw_texture_pro(buder_texture_t texture, buder_rect_t src, buder_rect_t dst, buder_vec2_t scale, buder_vec2_t origin, float angle, buder_color_t tint, int layer_index);
void bdr_draw_texture_rect(buder_texture_t texture, buder_rect_t src, buder_vec2_t position, buder_color_t tint, int layer_index);
void bdr_draw_texture(buder_texture_t texture, buder_vec2_t position, buder_color_t tint, int layer_index);
// exprimental sprite animation
buder_rect_t bdr_sprite_animation_update(buder_sprite_animation_t *animation);
buder_texture_t bdr_sprite_animation_get_texture(buder_sprite_animation_t *animation);
buder_vec2_t bdr_sprite_animation_get_size_frame(buder_sprite_animation_t *animation);
bool bdr_sprite_animation_is_finished(buder_sprite_animation_t *animation);
void bdr_sprite_animation_play(buder_sprite_animation_t *animation, int active);
void bdr_sprite_animation_set_speed(buder_sprite_animation_t *animation, int speed);
void bdr_sprite_animation_set_frame(buder_sprite_animation_t *animation, int frame);
// void bdr_sprite_animation_set_fps(buder_sprite_animation_t *animation, int fps);
void bdr_sprite_animation_register(buder_sprite_animation_t *animation, int key_id, buder_sprite_animation_frame_t frame);
void bdr_sprite_animation_unregister(buder_sprite_animation_t *animation, int key_id);
void bdr_sprite_animation_destroy(buder_sprite_animation_t *animation);

// MODULO FONTS y TEXT
buder_font_t bdr_load_font(const char *filename);
void bdr_free_font(buder_font_t font);
buder_font_t bdr_get_default_font(void);
void bdr_draw_text_pro(buder_font_t font, const char *text, float x, float y, float font_size, buder_vec2_t origin, buder_color_t color, int layer_index);
void bdr_draw_text(const char *text, float x, float y, float font_size, buder_color_t color, int layer_index);
int bdr_text_measure(buder_font_t font, const char *text, float font_size);

// MODULE SHAPE
void bdr_draw_rect(float x, float y, float w, float h, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void bdr_draw_line(float x0, float y0, float x1, float y1, float thickness, buder_color_t color, int layer_index);
void bdr_draw_circle(float x, float y, float radius, buder_color_t fill_color, buder_color_t outline_color, float outline_thickness, int layer_index);
void bdr_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, buder_color_t color, int layer_index);
void bdr_draw_grid(int width, int height, int cell_size, buder_color_t color, int layer_index);

// MODULE AUDIO
void bdr_play_sound(const char *filename);
// void bdr_play_sound(buder_sound_t sound);
// buder_sound_t bdr_load_sound(const char *filename);
// void bdr_free_sound(buder_sound_t sound);
// void bdr_pause_sound(buder_sound_t sound);
// void bdr_resume_sound(buder_sound_t sound);
// void bdr_stop_sound(buder_sound_t sound);
// void bdr_set_sound_volume(buder_sound_t sound, float volume);
// void bdr_set_sound_pitch(buder_sound_t sound, float pitch);
// void bdr_set_sound_loop(buder_sound_t sound, bool loop);

// MODULE COLOR
buder_color_t bdr_color_fade(buder_color_t color, float alpha);
uint32_t bdr_color_to_uint(buder_color_t color);

// MODULE MATH
float bdr_math_clamp(float value, float min, float max);
int bdr_math_random_int(int min, int max);

// MODULE STRING
char *bdr_string_format(const char *str, ...);
const char *bdr_string_strpbrk(const char *s, const char *charset);

// MODULE PATH, FILE
const char *bdr_file_name_get(const char *filePath);
char *bdr_file_name_without_ext(const char *filepath);

// MODULE INPUT AND EVENTS
void bdr_event_pool(buder_t *buder, const buder_event_t *event);
void bdr_event_mouse_position(buder_t *buder, float x, float y);
void bdr_event_mouse_wheel(buder_t *buder, float wheel_x, float wheel_y);
void bdr_event_keyboard_events(buder_t *buder, int key, int action);
void bdr_event_mouse_events(buder_t *buder, int mouse_button, int action);

int bdr_mouse_x(buder_t *buder);
int bdr_mouse_y(buder_t *buder);
buder_vec2_t bdr_mouse_get_pos(buder_t *buder);
bool bdr_mouse_is_down(buder_t *buder, buder_mousebutton button);
bool bdr_mouse_is_up(buder_t *buder, buder_mousebutton button);
bool bdr_mouse_is_pressed(buder_t *buder, buder_mousebutton button);
bool bdr_mouse_is_released(buder_t *buder, buder_mousebutton button);
float bdr_mouse_wheel_move(buder_t *buder);

bool bdr_keyboard_is_up(buder_t *buder, buder_keyboard key);
bool bdr_keyboard_is_down(buder_t *buder, buder_keyboard key);
bool bdr_keyboard_is_pressed(buder_t *buder, buder_keyboard key);
bool bdr_keyboard_is_released(buder_t *buder, buder_keyboard key);

// COLLISION DETECTION
bool bdr_collision_point_rec(buder_vec2_t point, buder_rect_t rec);
bool bdr_collision_rec_rec(buder_rect_t rec1, buder_rect_t rec2);
bool bdr_collision_point_circle(buder_vec2_t point, buder_vec2_t center, float radius);
bool bdr_collision_circle_rec(buder_vec2_t center, float radius, buder_rect_t rec);

#endif // BUDER_H
