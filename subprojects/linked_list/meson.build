project('linked_list', 'c', license : 'MIT', version : '1.0.0', default_options : ['c_std=c18'])

subdir('src')
include = include_directories('include')

linked_list = static_library('linked_list', sources, include_directories : include)
linked_list_dep = declare_dependency(include_directories : include, link_with : linked_list)
