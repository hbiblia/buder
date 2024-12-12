#include "../src/buder.h"

#define MAX_BUILDINGS   100

static buder_camera_t camera = {0};
static buder_rect_t player = { 400, 280, 40, 40 };
static buder_rect_t buildings[MAX_BUILDINGS] = { 0 };
static buder_color_t buildColors[MAX_BUILDINGS] = { 0 };

void bwindow_init(buder_t *buder)
{
    buder_set_stage_color(buder, BUDEWHITE);

    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
    // camera.offset = (buder_vec2_t){buder->width/2, buder->height/2};
    // camera.target = (buder_vec2_t){player.x + 20.0f, player.y + 20.0f};

    int spacing = 0;
    for(int i = 0; i< MAX_BUILDINGS; i++)
    {
        buildings[i].w = (float)buder_random_int(50, 200);
        buildings[i].h = (float)buder_random_int(100, 800);
        buildings[i].y = buder->height - 130.0f - buildings[i].h;
        buildings[i].x = -6000 + spacing;

        spacing += (int)buildings[i].w;

        buildColors[i] = (buder_color_t){buder_random_int(200, 240), buder_random_int(200, 240), buder_random_int(200, 250), 255};
    }
}

void bwindow_frame(buder_t *buder, float delta)
{
    camera.offset = (buder_vec2_t){buder->width/2, buder->height/2};
    camera.target = (buder_vec2_t){player.x + 20.0f, player.y + 20.0f};
    
    buder_begin_frame(buder);
    {
        buder_begin_camera(camera);
        {
            for(int i = 0; i< MAX_BUILDINGS; i++)
            {
                buder_draw_rect(buildings[i].x, buildings[i].y, buildings[i].w, buildings[i].h, buildColors[i], BLANK, 0, 0);
            }

            buder_draw_rect(player.x, player.y, player.w, player.h, RED, BLANK, 0, 0);
            
            buder_draw_rect(-6000, 320, 13000, 8000, DARKGRAY, BLANK, 0, 0);

            buder_draw_line(camera.target.x, -buder->height*10, camera.target.x, buder->height*10, 1, GREEN, 0);
            buder_draw_line(-buder->width*10, camera.target.y, buder->width*10, camera.target.y, 1, GREEN, 0);
        }
        buder_end_camera();

        buder_draw_textd("SCREEN AREA", buder->width-120, 20, 20, RED, 0);

        buder_draw_rect(0,0, buder->width, buder->height, BLANK, RED, 10, 0);
        buder_draw_rect(10, 10, 250, 113, buder_color_fade(SKYBLUE, 0.5f), BLUE, 1.0f, 0);

        buder_draw_textd("Free 2d camera controls:", 20, 25, 15, BLACK, 0);
        buder_draw_textd("- Right/Left to move Offset", 40, 45, 15, DARKGRAY, 0);
        buder_draw_textd("- Mouse Wheel to Zoom in-out", 40, 65, 15, DARKGRAY, 0);
        buder_draw_textd("- A / S to Rotate", 40, 85, 15, DARKGRAY, 0);
        buder_draw_textd("- R to reset Zoom and Rotation", 40, 105, 15, DARKGRAY, 0);
    }
    buder_end_frame(buder);
}

void bwindow_shutdown(void)
{
}

void bwindow_event(buder_t *buder, const buder_event_t *event)
{
    if(event->type == BUDER_EVENT_KEY_DOWN)
    {
        if(event->key == BUDER_KEY_RIGHT)
        {
            player.x += 2;
        }
        else if(event->key == BUDER_KEY_LEFT)
        {
            player.x -= 2;
        }

        if(event->key == BUDER_KEY_A)
        {
            camera.rotation++;
        }
        else if(event->key == BUDER_KEY_S)
        {
            camera.rotation--;
        }

        if(event->key == BUDER_KEY_R)
        {
            camera.zoom = 1.0f;
            camera.rotation = 0.0f;
        }
    }
    else if(event->type == BUDER_EVENT_MOUSE_SCROLL)
    {
        camera.zoom += event->scroll_y * 0.005f;
        camera.zoom = buder_math_clamp(camera.zoom, 0.1f, 3.0f);
    }
}
