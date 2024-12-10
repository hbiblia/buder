cc = meson.get_compiler('c')

deps = bdeps
deps += declare_dependency(link_with: libbuder)

links = []

#executable example compile sokol_app
executable('window_sokol', 'window_sokol.c', link_args:links, dependencies: deps, install: true, install_dir: base_dir + '/dist/')

#executable example compile glfw3
# executable('window_glfw3', 'window_glfw3.c', link_args:links, dependencies: deps, install: true, install_dir: base_dir + '/dist/')

