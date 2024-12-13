#include "../src/buder.h"

#define MAX_BUILDINGS 100
#define PLAYER_MOVE_SPEED 2.0f
#define CAMERA_ROTATION_SPEED 1.0f
#define CAMERA_DEFAULT_ZOOM 1.0f
#define CAMERA_DEFAULT_ROTATION 0.0f

static buder_camera_t camera = {0};
static buder_rect_t player = {400, 280, 40, 40};
static buder_rect_t buildings[MAX_BUILDINGS] = {0};
static buder_color_t buildColors[MAX_BUILDINGS] = {0};

void bwindow_init(buder_t *buder)
{
    bdr_viewport_color(buder, BUDEWHITE);

    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    int spacing = 0;
    for (int i = 0; i < MAX_BUILDINGS; i++)
    {
        buildings[i].w = (float)bdr_math_random_int(50, 200);
        buildings[i].h = (float)bdr_math_random_int(100, 800);
        buildings[i].y = buder->height - 130.0f - buildings[i].h;
        buildings[i].x = -6000 + spacing;

        spacing += (int)buildings[i].w;

        buildColors[i] = (buder_color_t){bdr_math_random_int(200, 240), bdr_math_random_int(200, 240), bdr_math_random_int(200, 250), 255};
    }
}

void bwindow_frame(buder_t *buder, float delta)
{
    // Diagonal movement prevention and smoother input handling
    int move_right = bdr_keyboard_is_down(buder, BUDER_KEY_RIGHT);
    int move_left = bdr_keyboard_is_down(buder, BUDER_KEY_LEFT);
    
    // Only move if one direction is pressed to prevent diagonal movement
    if (move_right && !move_left) {
        player.x += PLAYER_MOVE_SPEED;
    } 
    else if (move_left && !move_right) {
        player.x -= PLAYER_MOVE_SPEED;
    }

    // Camera rotation with more flexible controls
    if (bdr_keyboard_is_down(buder,BUDER_KEY_A)) {
        camera.rotation += CAMERA_ROTATION_SPEED;
    } 
    else if (bdr_keyboard_is_down(buder,BUDER_KEY_S)) {
        camera.rotation -= CAMERA_ROTATION_SPEED;
    } 
    
    // Reset camera with a single dedicated key
    if (bdr_keyboard_is_down(buder,BUDER_KEY_R)) {
        camera.zoom = CAMERA_DEFAULT_ZOOM;
        camera.rotation = CAMERA_DEFAULT_ROTATION;
    }

    camera.zoom += bdr_mouse_wheel_move(buder) * 0.005f;

    camera.offset = (buder_vec2_t){buder->width / 2, buder->height / 2};
    camera.target = (buder_vec2_t){player.x + 20.0f, player.y + 20.0f};
    camera.zoom = bdr_math_clamp(camera.zoom, 0.1f, 3.0f);

    // begin draw -- -- --
    bdr_camera_begin(camera);
    {
        for (int i = 0; i < MAX_BUILDINGS; i++)
        {
            bdr_draw_rect(buildings[i].x, buildings[i].y, buildings[i].w, buildings[i].h, buildColors[i], BLANK, 0, 0);
        }

        bdr_draw_rect(player.x, player.y, player.w, player.h, RED, BLANK, 0, 0);
        bdr_draw_rect(-6000, 320, 13000, 8000, DARKGRAY, BLANK, 0, 0);
        bdr_draw_line(camera.target.x, -buder->height * 10, camera.target.x, buder->height * 10, 1, GREEN, 0);
        bdr_draw_line(-buder->width * 10, camera.target.y, buder->width * 10, camera.target.y, 1, GREEN, 0);
    }
    bdr_camera_end();

    bdr_draw_rect(0, 0, buder->width, buder->height, BLANK, RED, 10, 0);
    bdr_draw_rect(10, 10, 250, 113, bdr_color_fade(SKYBLUE, 0.5f), BLUE, 1.0f, 0);

    bdr_draw_text("SCREEN AREA", buder->width - 120, 20, 20, RED, 0);
    bdr_draw_text("Free 2d camera controls:", 20, 25, 15, BLACK, 0);
    bdr_draw_text("- Right/Left to move Offset", 40, 45, 15, DARKGRAY, 0);
    bdr_draw_text("- Mouse Wheel to Zoom in-out", 40, 65, 15, DARKGRAY, 0);
    bdr_draw_text("- A / S to Rotate", 40, 85, 15, DARKGRAY, 0);
    bdr_draw_text("- R to reset Zoom and Rotation", 40, 105, 15, DARKGRAY, 0);

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
}

