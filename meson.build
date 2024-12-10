project('buder', 'c',
  version : '0.1',
  default_options : ['warning_level=0'])
  
base_dir = meson.current_source_dir()
cc = meson.get_compiler('c')

add_project_arguments('-DSOKOL_GLCORE', language: 'c')

bdeps = []
# win openGL
bdeps += cc.find_library('kernel32')
bdeps += cc.find_library('user32')
bdeps += cc.find_library('shell32')
bdeps += cc.find_library('gdi32')
bdeps += cc.find_library('ole32')

subdir('src')
subdir('examples')


