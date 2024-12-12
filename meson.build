project('buder', 'c',
  version : '0.1',
  default_options : ['renderer_selected=glcore','warning_level=0'])
  
base_dir = meson.current_source_dir()
cc = meson.get_compiler('c')
host_system = host_machine.system()

renderer_selected = get_option('renderer_selected')

# Definiciones de backends
sokol_defines = {
  'glcore': '-DSOKOL_GLCORE',
  'gles3': '-DSOKOL_GLES3', 
  'd3d11': '-DSOKOL_D3D11',
  'metal': '-DSOKOL_METAL',
  'wgpu': '-DSOKOL_WGPU'
}

message('Renderer selected: ' + sokol_defines[renderer_selected])

bdeps = []

if host_system == 'windows'
  bdeps += cc.find_library('kernel32')
  bdeps += cc.find_library('user32')
  bdeps += cc.find_library('shell32')
  bdeps += cc.find_library('gdi32')
  bdeps += cc.find_library('ole32')

 if renderer_selected == 'd3d11'
  bdeps += cc.find_library('dxgi')
  bdeps += cc.find_library('d3d11')
  endif
endif

add_project_arguments(sokol_defines[renderer_selected], language: 'c')

subdir('src')
subdir('examples')


