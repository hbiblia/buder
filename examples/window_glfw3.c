
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../src/buder.h"

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "GLFW + Buder", NULL, NULL);

    buder_t buder;
    buder_init(&buder);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        buder_set_size(&buder, width, height);
        buder_begin_frame(&buder);
        {
            // Unos rectangulos de diferentes colores y tamaños, los borders son diferentes colores.
            buder_draw_rect(100, 100, 100, 100, (buder_color_t){255, 0, 0, 255}, (buder_color_t){0, 0, 255, 255}, 5, 0);
            buder_draw_rect(200, 200, 100, 100, (buder_color_t){0, 255, 0, 255}, (buder_color_t){255, 0, 0, 255}, 10, 0);

            // Unas lineas cortas de diferentes colores
            buder_draw_line(0, 0, 100, 100, 1, (buder_color_t){255, 0, 0, 255}, 0);
            buder_draw_line(0, 0, 100, 200, 5, (buder_color_t){0, 255, 0, 255}, 0);

            // Unos circulos de diferentes colores, , los borders son diferentes colores.
            buder_draw_circle(500, 200, 50, (buder_color_t){0, 0, 255, 255}, (buder_color_t){255, 0, 0, 255}, 5, 0);
            buder_draw_circle(600, 200, 50, (buder_color_t){255, 0, 0, 255}, (buder_color_t){0, 255, 0, 255}, 1, 0);

            // Unos triangulos de diferentes colores [azul, rojo], uno del lado del otro.
            buder_draw_triangle(500, 500, 600, 500, 550, 600, (buder_color_t){0, 0, 255, 255}, 0);
            buder_draw_triangle(600, 500, 700, 500, 650, 600, (buder_color_t){255, 0, 0, 255}, 0);
        }
        buder_end_frame(&buder);
    }

    buder_shutdown(&buder);
    glfwTerminate();
}
