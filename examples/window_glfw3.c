
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../src/buder.h"

void bwindow_init(buder_t *buder);
void bwindow_frame(buder_t *buder, float delta);
void bwindow_event(buder_t *buder, const buder_event_t *event);
void bwindow_shutdown(void);

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Example Buder + GLFW3", NULL, NULL);

    static buder_t buder;

    buder_init(&buder);
    bwindow_init(&buder);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        buder_set_stage_size(&buder, width, height);
        bwindow_frame(&buder, 0.0f);
    }

    bwindow_shutdown();
    buder_shutdown(&buder);
    
    glfwTerminate();
}
