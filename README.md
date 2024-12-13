# Buder  
Una biblioteca experimental para crear juegos 2D.  

## Integración  
Buder está diseñado para integrarse fácilmente con cualquier sistema de ventanas, incluyendo **sokol_app**, **GLFW3**, **SDL2**, **SDL3**, **GTK**, y más.

## Ejemplo

```c
buder_t buder;
buder_init(&buder, 800, 600);

const int twdth = bdr_text_measure(bdr_get_default_font(), "Example text", 25);
bdr_draw_text("Example text", buder.width/2 - twdth/2, 70, 25, PINK, 0);

buder_shutdown(&buder);

```