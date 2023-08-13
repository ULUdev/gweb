project('gweb', 'c', license : 'MIT', version : '1.5.7', default_options : ['c_std=c18'])

subdir('src')
cc = meson.get_compiler('c')
add_project_arguments('-D_XOPEN_SOURCE=600', language : 'c')
include = include_directories('include')

if not cc.has_function('realpath')
  error('gweb requires an implementation of realpath and doesn\'t provide one itself')
endif

gtk = dependency('gtk+-3.0')
webkitgtk = dependency('webkit2gtk-4.0')
linked_list = dependency('linked_list', fallback : ['linked_list', 'linked_list_dep'])
hashmap = dependency('hashmap', fallback : ['hashmap', 'hashmap_dep'])


deps = [gtk, webkitgtk, linked_list, hashmap]
gweb = executable('gweb', sources, include_directories : include, install : true, dependencies : deps)

test('version', gweb, args : ['-V'])
test('help', gweb, args : ['-h'])

install_data('gweb.desktop', install_dir : get_option('datadir') / 'applications')
install_data('gweb.png', install_dir : get_option('datadir') / 'pixmaps')
