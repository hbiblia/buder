cc = meson.get_compiler('c')

deps = bdeps

sources = [
    'buder.c'
]

headers = [
    'buder.h'
]

include_directs = include_directories(['.', 'libraries/'])

# compile una libreria dll
# share_libbuder = shared_library('buder', sources, dependencies: deps, include_directories: include_directs, install: true)
libbuder = static_library('buder', sources, dependencies: deps, include_directories: include_directs, install: true, install_dir: base_dir + '/dist/')

# Instalar headers
install_headers(headers)

