# Buder  

**Buder** es una biblioteca experimental para la creación de juegos 2D. Está inspirada en **raylib**, y varios ejemplos se han reescrito para funcionar con Buder.

## Características  

## Integración  

Buder se integra fácilmente con múltiples sistemas de ventanas, como:  
- **sokol_app**  
- **GLFW3**  
- **SDL2** y **SDL3**  
- **GTK**  
- ¡y más!  

Esto permite adaptarla a cualquier flujo de trabajo o plataforma.

## Ejemplo de uso  

El siguiente ejemplo muestra cómo inicializar Buder, medir el ancho de un texto y dibujarlo centrado en la pantalla:

```c
#include "buder.h"

int main() {
    buder_t buder;

    GLFWwindow *window = glfwCreateWindow(800, 600, "Example Buder + GLFW3", NULL, NULL);

    buder_init(&buder, 800, 600);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        const int text_width = bdr_text_measure(bdr_get_default_font(), "Example text", 25);
        bdr_draw_text("Example text", buder.width / 2 - text_width / 2, 70, 25, PINK, 0);
        
        bdr_viewport_present(buder);
    }

    buder_shutdown(&buder); // Cierra Buder y libera recursos
    glfwTerminate();

    return 0;
}
