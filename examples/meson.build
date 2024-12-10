cc = meson.get_compiler('c')

deps = bdeps
deps += declare_dependency(link_with: libbuder)

links = []

#executable example compile
executable('window', 'window.c', link_args:links, dependencies: deps, install: true, install_dir: base_dir + '/dist/')

