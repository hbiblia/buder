#include "../src/buder.h"
#include <stdio.h>

#define SNAKE_SIZE 256
#define FOOD_SIZE 20
#define GRID_SIZE 20
#define SNAKE_FACE 0

typedef struct Actor
{
    buder_vec2_t position;
    buder_vec2_t velocity;
    buder_vec2_t size;
    buder_color_t background;
    bool enabled;
} Actor;

static Actor Snake[SNAKE_SIZE] = {0};
static int snakeCounterTail = 1;
static buder_vec2_t snakePositionTail[SNAKE_SIZE] = {0};
static int counter_frame;

static Actor Food = {0};

static bool gameOver = false;
static bool pause = false;

static void game_over_init(int width, int height);

// ---------------------------------------------------------
// SNAKE
// ---------------------------------------------------------
static void snake_init(int width, int height)
{
    for (int i = 0; i < SNAKE_SIZE; i++)
    {
        Snake[i].position = (buder_vec2_t){width / 2, height / 2};
        Snake[i].velocity = (buder_vec2_t){0, 0};
        Snake[i].size = (buder_vec2_t){GRID_SIZE, GRID_SIZE};
        Snake[i].background = MAGENTA;
        Snake[i].enabled = false;
        snakePositionTail[i] = (buder_vec2_t){0, 0};
    }

    snakeCounterTail = 1;
    counter_frame = 0;
}

static void snake_update(int width, int height)
{
    for (int i = 0; i < snakeCounterTail; i++)
    {
        snakePositionTail[i] = Snake[i].position;
        Snake[i].background = i == 0 ? BLUE : DARKBLUE;
    }

    if ((counter_frame % GRID_SIZE) == 0)
    {
        for (int i = 0; i < snakeCounterTail; i++)
        {
            if (i == SNAKE_FACE)
            {
                Snake[i].position.x += Snake[i].velocity.x;
                Snake[i].position.y += Snake[i].velocity.y;
                Snake[i].enabled = true;
            }
            else
                Snake[i].position = snakePositionTail[i - 1];
        }
    }

    // Verificar si la serpiente choca consigo misma
    for (int i = 1; i < snakeCounterTail; i++)
    {
        if (Snake[SNAKE_FACE].position.x == Snake[i].position.x && Snake[SNAKE_FACE].position.y == Snake[i].position.y)
        {
            game_over_init(width, height);
        }
    }

    // Verificar si la serpiente choca con los bordes
    if (Snake[SNAKE_FACE].position.x >= width || Snake[SNAKE_FACE].position.x < 0 || Snake[SNAKE_FACE].position.y >= height || Snake[SNAKE_FACE].position.y < 0)
    {
        game_over_init(width, height);
    }

    counter_frame++;
}

static void snake_draw(void)
{
    for (int i = 0; i < snakeCounterTail; i++)
    {
        buder_draw_rect(Snake[i].position.x, Snake[i].position.y, Snake[i].size.x, Snake[i].size.y, Snake[i].background, BLANK, 0, 0);
    }
}

// ---------------------------------------------------------
// FOOD
// ---------------------------------------------------------
static void food_update_position(int width, int height)
{
    Food.position = (buder_vec2_t){((float)buder_random_int(0, (width / FOOD_SIZE) - 1) * FOOD_SIZE), ((float)buder_random_int(0, (height / FOOD_SIZE) - 1) * FOOD_SIZE)};
}

static void food_init(void)
{
    Food.size = (buder_vec2_t){FOOD_SIZE, FOOD_SIZE};
    Food.background = BUDEWHITE;
}

static void food_update(int width, int height)
{
    // cuando la comida no esta habilitada, se actualiza su posicion.
    if (!Food.enabled)
    {
        food_update_position(width, height);
        Food.enabled = true;

        for (int i = 0; i < snakeCounterTail; i++)
        {
            if (Snake[i].position.x == Food.position.x && Snake[i].position.y == Food.position.y)
            {
                food_update_position(width, height);
            }
        }
    }

    // Verificar si alguna parte de la serpiente come la comida
    for (int i = 0; i < snakeCounterTail; i++)
    {
        if (Snake[i].position.x == Food.position.x && Snake[i].position.y == Food.position.y)
        {
            Snake[snakeCounterTail].position = snakePositionTail[snakeCounterTail - 1];
            snakeCounterTail += 1;
            Food.enabled = false;
            buder_play_sound("resources/fruit-hit.wav");
        }
    }
}

static void food_draw()
{
    buder_draw_rect(Food.position.x, Food.position.y, Food.size.x, Food.size.y, Food.background, BLANK, 0, 0);
}

// ---------------------------------------------------------
// GameObver
// ---------------------------------------------------------
static void game_over_init(int width, int height)
{
    gameOver = true;
    snake_init(width, height);
}

static void game_over(int width, int height)
{
    buder_draw_text("\n\tGame Over", width / 2, height / 2, RED, 0);
    buder_draw_text("\n\n\tPress Space to Restart", width / 2, height / 2, RED, 0);
}

// ---------------------------------------------------------
// Game
// ---------------------------------------------------------
void bwindow_init(buder_t *buder)
{
    snake_init(buder->width, buder->height);
    food_init();
}

void bwindow_frame(buder_t *buder, float delta)
{
    snake_update(buder->width, buder->height);
    food_update(buder->width, buder->height);

    buder_begin_frame(buder);
    {
        if (gameOver)
        {
            game_over(buder->width, buder->height);
        }else {
            buder_draw_text("\n\tMove the Snake Up, Down, Left, or Right.", buder->width / 2, buder->height / 2, WHITE, 0);
            food_draw();
        }

        snake_draw();
    }
    buder_end_frame(buder);
}

void bwindow_event(buder_t *buder, const buder_event_t *event)
{
    if (event->type == BUDER_EVENT_KEY_DOWN)
    {
        if (event->key == BUDER_KEY_SPACE && gameOver == true)
        {
            gameOver = false;
        }
        else
        {
            if (event->key == BUDER_KEY_UP && (Snake[SNAKE_FACE].velocity.y == 0) && Snake[SNAKE_FACE].enabled)
            {
                Snake[SNAKE_FACE].velocity = (buder_vec2_t){0, -GRID_SIZE};
                Snake[SNAKE_FACE].enabled = false;
            }

            if (event->key == BUDER_KEY_DOWN && (Snake[SNAKE_FACE].velocity.y == 0) && Snake[SNAKE_FACE].enabled)
            {
                Snake[SNAKE_FACE].velocity = (buder_vec2_t){0, GRID_SIZE};
                Snake[SNAKE_FACE].enabled = false;
            }

            if (event->key == BUDER_KEY_LEFT && (Snake[SNAKE_FACE].velocity.x == 0) && Snake[SNAKE_FACE].enabled)
            {
                Snake[SNAKE_FACE].velocity = (buder_vec2_t){-GRID_SIZE, 0};
                Snake[SNAKE_FACE].enabled = false;
            }

            if (event->key == BUDER_KEY_RIGHT && (Snake[SNAKE_FACE].velocity.x == 0) && Snake[SNAKE_FACE].enabled)
            {
                Snake[SNAKE_FACE].velocity = (buder_vec2_t){GRID_SIZE, 0};
                Snake[SNAKE_FACE].enabled = false;
            }
        }
    }
}

void bwindow_shutdown(void)
{
}