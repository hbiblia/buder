

bdr_actor_init();

bdr_resource_load_texture("texture", "path/to/texture.png");

Actor *stage = bdr_actor_stage_new();

Actor *sprite = bdr_actor_sprite_new();
bdr_actor_sprite_set_texture(sprite, "texture");
bdr_actor_stage_add(stage, sprite);

int length = bdr_actor_children_length(scene);
GList *children = bdr_actor_children(scene);

bdr_actor_main(scene);


