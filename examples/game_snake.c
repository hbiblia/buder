#include "../src/buder.h"
#include <stdio.h>
#include <stdbool.h>

#define SNAKE_MAX_LENGTH 256
#define GRID_SIZE 20
#define SNAKE_FACE 0

typedef struct
{
    buder_vec2_t position;
    buder_vec2_t velocity;
    buder_vec2_t size;
    buder_color_t color;
    bool enabled;
} Actor;

static buder_vec2_t snake_trail[SNAKE_MAX_LENGTH] = {0};
static Actor snake_body[SNAKE_MAX_LENGTH] = {0};
static int snake_length = 1;
static int frame_counter = 0;
static int snake_score = 0;
static Actor food = {0};
static bool game_over = false;

bool is_valid_movement(buder_vec2_t current_velocity, buder_vec2_t new_velocity)
{
    return (current_velocity.x != -new_velocity.x) &&
           (current_velocity.y != -new_velocity.y);
}

bool is_position_valid(buder_vec2_t position, Actor *snake, int snake_length)
{
    for (int i = 0; i < snake_length; i++)
    {
        if (position.x == snake[i].position.x &&
            position.y == snake[i].position.y)
        {
            return false;
        }
    }
    return true;
}

static void place_food(int width, int height)
{
    do
    {
        food.position = (buder_vec2_t){
            ((float)bdr_math_random_int(0, (width / GRID_SIZE) - 1) * GRID_SIZE),
            ((float)bdr_math_random_int(0, (height / GRID_SIZE) - 1) * GRID_SIZE)};
    } while (!is_position_valid(food.position, snake_body, snake_length));

    food.enabled = true;
}

static void initialize_snake(int width, int height)
{
    for (int i = 0; i < SNAKE_MAX_LENGTH; i++)
    {
        snake_body[i] = (Actor){
            .position = {(width / GRID_SIZE / 2) * GRID_SIZE, (height / GRID_SIZE / 2) * GRID_SIZE},
            .velocity = {0, 0},
            .size = {GRID_SIZE, GRID_SIZE},
            .color = (i == 0) ? BLUE : DARKBLUE,
            .enabled = false};
        snake_trail[i] = (buder_vec2_t){0, 0};
    }

    snake_body[SNAKE_FACE].velocity = (buder_vec2_t){GRID_SIZE, 0};

    snake_length = 1;
    snake_score = 0;
    frame_counter = 0;
}

static void update_snake_position(int width, int height, float delta)
{
    for (int i = 0; i < snake_length; i++)
    {
        snake_trail[i] = snake_body[i].position;
        snake_body[i].color = (i == 0) ? BLUE : DARKBLUE;
    }

    if ((frame_counter % GRID_SIZE) == 0)
    {
        for (int i = snake_length - 1; i > 0; i--)
        {
            snake_body[i].position = snake_body[i - 1].position;
        }

        snake_body[SNAKE_FACE].position.x += snake_body[SNAKE_FACE].velocity.x;
        snake_body[SNAKE_FACE].position.y += snake_body[SNAKE_FACE].velocity.y;
        snake_body[SNAKE_FACE].enabled = true;
    }

    buder_vec2_t head_pos = snake_body[SNAKE_FACE].position;
    if (head_pos.x >= width || head_pos.x < 0 ||
        head_pos.y >= height || head_pos.y < 0)
    {
        game_over = true;
    }

    for (int i = 1; i < snake_length; i++)
    {
        if (head_pos.x == snake_body[i].position.x &&
            head_pos.y == snake_body[i].position.y)
        {
            game_over = true;
            break;
        }
    }

    frame_counter += 1;
}

static void update_food(int width, int height)
{
    if (!food.enabled)
    {
        place_food(width, height);
    }

    for (int i = 0; i < snake_length; i++)
    {
        if (snake_body[i].position.x == food.position.x &&
            snake_body[i].position.y == food.position.y)
        {
            snake_body[snake_length].position = snake_trail[snake_length - 1];
            snake_length++;
            snake_score++;
            food.enabled = false;
            // bdr_play_sound("resources/fruit-hit.wav");
            break;
        }
    }
}

static void draw_snake(void)
{
    for (int i = 0; i < snake_length; i++)
    {
        bdr_draw_rect(
            snake_body[i].position.x,
            snake_body[i].position.y,
            snake_body[i].size.x,
            snake_body[i].size.y,
            snake_body[i].color,
            BLANK, 0, 0);
    }
}

static void draw_food(void)
{
    bdr_draw_rect(
        food.position.x,
        food.position.y,
        food.size.x,
        food.size.y,
        food.color,
        BLANK, 0, 0);
}

void bwindow_init(buder_t *buder)
{
    initialize_snake(buder->width, buder->height);

    food.size = (buder_vec2_t){GRID_SIZE, GRID_SIZE};
    food.color = BUDEWHITE;
}

void bwindow_frame(buder_t *buder, float delta)
{
    if (bdr_keyboard_is_down(buder, BUDER_KEY_SPACE) && game_over)
    {
        game_over = false;
        initialize_snake(buder->width, buder->height);
        place_food(buder->width, buder->height);
    }
    else if (!game_over)
    {
        Actor *head = &snake_body[SNAKE_FACE];
        bool can_change_direction = head->enabled;
        buder_vec2_t new_velocity = head->velocity;

        bool is_key_up = bdr_keyboard_is_down(buder, BUDER_KEY_UP) || bdr_keyboard_is_down(buder, BUDER_KEY_W);
        bool is_key_down = bdr_keyboard_is_down(buder, BUDER_KEY_DOWN) || bdr_keyboard_is_down(buder, BUDER_KEY_S);
        bool is_key_left = bdr_keyboard_is_down(buder, BUDER_KEY_LEFT) || bdr_keyboard_is_down(buder, BUDER_KEY_A);
        bool is_key_right = bdr_keyboard_is_down(buder, BUDER_KEY_RIGHT) || bdr_keyboard_is_down(buder, BUDER_KEY_D);

        if (is_key_up && head->velocity.y == 0)
        {
            new_velocity = (buder_vec2_t){0, -GRID_SIZE};
        }
        else if (is_key_down && head->velocity.y == 0)
        {
            new_velocity = (buder_vec2_t){0, GRID_SIZE};
        }
        else if (is_key_left && head->velocity.x == 0)
        {
            new_velocity = (buder_vec2_t){-GRID_SIZE, 0};
        }
        else if (is_key_right && head->velocity.x == 0)
        {
            new_velocity = (buder_vec2_t){GRID_SIZE, 0};
        }

        if (is_valid_movement(head->velocity, new_velocity) && can_change_direction)
        {
            head->velocity = new_velocity;
            head->enabled = false;
        }
    }

    update_snake_position(buder->width, buder->height, delta);
    update_food(buder->width, buder->height);

    // begin draw -- -- --
    if (game_over)
    {
        int text_width = 0;
        float center_x = buder->width / 2;
        float center_y = buder->height / 2;

        const char* instructions1 = "Game Over";
        const int t1_width = bdr_text_measure(bdr_get_default_font(), instructions1, 30);
        bdr_draw_text(instructions1, center_x - t1_width/2, center_y, 30, RED, 0);

        const char* instructions2 = "Press [Space] to Restart";
        const int t2_width = bdr_text_measure(bdr_get_default_font(), instructions2, 20);
        bdr_draw_text(instructions2, center_x - t2_width/2, center_y + 20, 20, PINK, 0);
    }
    else
    {
        draw_food();
        draw_snake();

        bdr_draw_grid(buder->width, buder->height, GRID_SIZE, GRIDBLACK, 0);
        bdr_draw_text(bdr_string_format("Score: %d", snake_score), 10, 20, 20, WHITE, 0);

        const char* instructions = "Move the Snake Up, Down, Left, or Right.";
        const int t1_width = bdr_text_measure(bdr_get_default_font(), instructions, 20);
        bdr_draw_text(instructions, buder->width / 2 - t1_width/2, buder->height - 30, 20, WHITE, 0);
    }

    bdr_viewport_present(buder);
}

void bwindow_shutdown(void)
{
    // Liberar recursos si es necesario
}
